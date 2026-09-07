# SpudLib

Hardware Abstraction Layer (HAL) for the Apricot CAD/BIM stack. This is a from-scratch
translation layer over GPU, file system, memory, and network primitives — a general HAL,
not a GPU library with utilities bolted on. See `../CLAUDE.md` (the `eqdev` workspace
root) for how this repo fits into the larger Apricot architecture; this file covers
conventions specific to working inside `spudlib` itself.

## The one rule everything else follows

**SpudLib is pure mechanism, zero policy.** It translates a platform/GPU API into a
consistent C interface and stops there. It never makes a decision on the caller's
behalf, never has a default that hides a choice, and holds no global state.

- Expose enumeration/query functions so the caller (ApricotFields) can inspect and
  decide — `spudgpu_enumerate_devices`, not `spudgpu_select_best_device`.
- No "convenience" fallback that picks something for the caller. If a change feels
  like it wants one, that decision belongs in ApricotFields, not here.
- Everything flows through explicit handles the caller owns and passes in — no
  library-owned globals.
- If a bug turns out to be SpudLib silently defaulting instead of translating a
  caller-supplied parameter (e.g. `spudgpu_create_image_view` once hardcoded
  `VK_IMAGE_ASPECT_COLOR_BIT` regardless of what the caller asked for), that's a
  design-principle violation, not just a bug — fix it by passing the real parameter
  through, not by adding a special case.
- When a fix spans SpudLib and ApricotFields, make the correct structural change on
  both sides rather than working around a layering gap from one side only.

## Module map

Every module returns the shared `SPUDRESULT` enum (`spudcore.h`, ~90 named error
codes) rather than inventing its own error convention.

| Module | Header | Prefix | Backends |
|---|---|---|---|
| SpudGPU | `spudgpu.h` | `spudgpu_` | Vulkan, D3D12, Metal (scaffolded, unimplemented) |
| SpudFiles | `spudfiles.h` | `sfs_` | Windows only |
| SpudMemory | `spudmemory.h` | `smem_` | Windows, Linux |
| SpudNet | `spudnet.h` | `spudnet_` | Windows, Linux |
| SpudCore | `spudcore.h` | `spud_` / `SPUDRESULT` | platform-agnostic |
| SpudPerf | `spudperf.h` | `spudperf_` | Windows, Linux |

SpudGPU dominates the codebase (92 functions across 20 opaque handles) and is where
most work happens. The others are small, focused translation shims — don't let their
API surface grow beyond what they are: SpudNet is "move bytes," not a networking
framework (message framing, snapshot/delta semantics, host authority are all
ApricotFields'/Erethal's problem); SpudMemory is reserve/commit/decommit + an arena
allocator, not a general allocator library.

## SpudGPU conventions

- **Opaque handles only.** Every resource type (`spudgpu_instance`, `spudgpu_device`,
  `spudgpu_buffer`, `spudgpu_command_list`, ...) is a forward-declared pointer with no
  caller-visible fields. Backend-private struct layout lives in an internal header per
  backend (`spudgpuvulkan.h`, `spudgpud3d12.hpp`) — never in the public `spudgpu.h`.
- **Dynamic rendering, not render-pass objects, as the default path.** The renderpass
  concept was removed in favor of `spudgpu_cmd_begin_rendering`/`cmd_end_rendering`
  (commit `e90f921`). Don't reintroduce a `VkRenderPass`/`VkFramebuffer`-style object
  into the public API as the default — D3D12's backend already emulates this begin/end
  shape on top of its native render-target-view binding, not the other way around.
  If/when SpudLib targets hardware that lacks Vulkan 1.3/`VK_KHR_dynamic_rendering`
  (older/low-end Android, Wear OS smartwatches), the classic `VkRenderPass`/
  `VkFramebuffer` fallback this implies is reached only through `SPUDGPU_LEGACY_*`
  (see below) — `spudgpu_cmd_begin_rendering`/`spudgpu_rendering_begin_desc` keep an
  identical calling convention on both paths, but the fallback itself is exposed as a
  small opaque `spudgpu_framebuffer` handle (`SPUDGPU_LEGACY_FRAMEBUFFER`-gated,
  paired with `spudgpu_cmd_begin_rendering_legacy`) rather than hidden entirely
  inside the Vulkan backend. That object needs a real owner for its lifetime
  (creation, caching, invalidation on resize) — a private SpudLib-side cache keyed on
  attachment sets would be exactly the hidden global state/hidden decision the
  zero-policy rule above forbids. The owner is ApricotFields: an
  `aprend_framebuffer`-shaped wrapper that holds raw image views on modern hardware
  or a cached `spudgpu_framebuffer` on legacy hardware, presenting one uniform
  begin/end call to the rest of Aprend's rendering code either way.
- **`SPUDGPU_EXT_*` gates a capability gap between backends; `SPUDGPU_LEGACY_*`
  gates one extra caller-owned object for constrained hardware within a backend.**
  Both are compile-time macros in `spudgpu.h`, computed once and gated on everywhere
  else — never scatter a raw `#if !SPUDGPU_COMPILE_<BACKEND>` check across call
  sites; if a second backend later lacks the same thing, that should be a one-line
  edit to the macro's own definition, not an audit of every use site. They answer
  different questions and must not be conflated:
  - `SPUDGPU_EXT_<NAME>` gates a capability that some `GRAPHICS_BACKEND` choices
    don't implement at all (bindless/descriptor indexing is the reference example —
    Metal needs a `MTLHeap`-backed allocator it doesn't have yet). Define it `1` only
    when a backend that implements it is the one compiled in
    (`SPUDGPU_COMPILE_VULKAN_API || SPUDGPU_COMPILE_D3D12_API`-style), `0` otherwise,
    and wrap the entire public function/type group in `#if SPUDGPU_EXT_<NAME>` so
    calling it against a build that lacks it is a compile/link error, not a silent
    no-op or a runtime NULL surprise discovered on the wrong platform. Pair every
    `SPUDGPU_EXT_<NAME>` with a `SPUDRESULT_GPU_EXT_<NAME>_NOT_SUPPORTED` in
    `spudcore.h`, returned when the backend compiles the extension in but the
    specific device/driver still doesn't support it — a distinct, later-discovered
    case from the macro itself being `0`. Ray tracing, mesh shading, and
    `SPUDGPU_EXT_DESCRIPTOR_SETS`/`SPUDGPU_EXT_SUBPASS_MERGING` (see the OpenGL note
    below) are the next likely candidates whenever their real API surface gets
    built. An `EXT` has to stay a narrow, self-contained island (a handful of
    types/functions) — if gating a capability would mean wrapping most of
    `spudgpu_cmd_*` or another load-bearing chunk of the header, that's a sign the
    backend needs its own `GRAPHICS_BACKEND` entry instead of an `EXT` flag draped
    over nearly everything.
  - `SPUDGPU_LEGACY_<NAME>` gates a fallback for hardware that some devices *within*
    a single backend's target range lack, even though every *currently* targeted
    device of that backend has it (dynamic rendering on old/low-end Vulkan hardware
    is the motivating case). This is a runtime device fact, not a `GRAPHICS_BACKEND`
    choice — the same compiled Vulkan backend has to run on both old and new
    hardware — so `SPUDGPU_LEGACY_<NAME>` must gate an opt-in build configuration for
    targeting that constrained hardware profile (e.g. a dedicated CMake option),
    never `GRAPHICS_BACKEND` itself. Which path a given device actually needs is
    surfaced to the caller as a capability query (mirroring
    `spudgpu_bindless_capabilities::supported`), never resolved silently inside
    SpudLib — the caller decides which path to take per-device; SpudLib only reports
    the fact and does the mechanical object construction for whichever path is
    chosen. `SPUDGPU_LEGACY_<NAME>` may grow the public surface by exactly one
    narrow, opaque resource type when the fallback needs a persistent object with
    real lifetime (`SPUDGPU_LEGACY_FRAMEBUFFER` above) — that's still fundamentally
    different from `EXT`: the caller isn't adapting its behavior, it's just holding
    the handle SpudLib needs to do the identical job on older hardware.
- **OpenGL/OpenGL ES, if ever needed, is a fourth `GRAPHICS_BACKEND`
  (`SPUDGPU_COMPILE_OPENGL_API`), not reached through `SPUDGPU_LEGACY_*`.** Its gap
  from Vulkan/D3D12/Metal isn't one narrow fallback object — no true deferred
  command-list recording, no descriptor-set object, different synchronization
  primitives — which is a structural, broad difference, not a hardware-SKU fact
  within an existing backend. Express it the same way Metal's missing bindless is
  expressed today: a pile of `SPUDGPU_EXT_<NAME>` macros going to `0` for it
  (`SPUDGPU_EXT_DESCRIPTOR_SETS` is the clearest candidate), computed alongside
  Vulkan/D3D12/Metal as a genuine fourth backend choice — never squeezed under
  `SPUDGPU_LEGACY_*`'s opt-in constrained-hardware path, which exists for one small
  caller-owned object, not a structurally different API.
- **Backend file split mirrors the header, not guessed.** Each backend (Vulkan, D3D12,
  Metal) uses the same ten-file-per-concern layout: `context`, `buffer`, `image`,
  `shader`, `swapchain`, `descriptors`, `command`, `renderpass`, `native`, `sync`. If
  `spudgpu.h` grows a new concern, split a new file for it in every backend rather than
  dumping new functions into an existing one.
- **Native escape hatches are interop-only.** `spudgpu_vulkan_natives.h` /
  `_d3d12_natives.h` / `_metal_natives.h` unwrap a handle back to its raw native type
  (`VkInstance`, `ID3D12Device14`, ...) for third-party libraries SpudLib doesn't wrap
  itself (ImGui, RenderDoc). SpudLib has no visibility into what the caller does with
  the handle afterward — that's the point, don't add tracking/validation around it.
- **Metal's natives header is not a 1:1 mirror of Vulkan/D3D12** — these asymmetries
  are deliberate, worked out before any Metal implementation exists, don't "fix" them
  later thinking they're gaps:
  - No instance-equivalent accessor (`MTLCopyAllDevices()` replaces it)
  - No physical/logical device split (`id<MTLDevice>` covers both)
  - No queue-family-index accessor (Metal queues aren't partitioned into families)
  - No command-allocator accessor (command buffers come fresh from the queue each time)
  - Buffer/image views aren't distinct native types (raw offset/stride binding; a
    texture view is just another `id<MTLTexture>`)
  - No root-signature accessor (binding indices come from `[[buffer(n)]]`/
    `[[texture(n)]]` shader attributes)
  - Fences map to `id<MTLSharedEvent>` (needs CPU wait/signal/read plus
    cross-process sharing); semaphores map to plain `id<MTLEvent>` (GPU-only
    wait/signal across queues, which is all `spudgpu_semaphore`'s public API
    ever asks for). Neither uses `MTLFence` — it can't be waited on from the
    CPU or across command queues, and belongs in `spudgpu_cmd_pipeline_barrier`'s
    implementation instead, not in fence/semaphore.

## Backend status

- **Vulkan** — reference backend, plain C23, complete. Build against this first;
  D3D12 and Metal follow its shape.
- **D3D12** — complete, C++26. Cross-compiles the project's SPIR-V shaders to HLSL at
  runtime via **SPIRV-Cross**, built from source through `FetchContent` (deliberately
  not the Vulkan SDK's prebuilt libs — those are release-only `/MD` and mismatch a
  `/MDd` debug build; building from source lets it inherit `CMAKE_MSVC_RUNTIME_LIBRARY`
  correctly). Also pulls in Microsoft's vendored `d3dx12.h` (3,400+ lines, not
  authored here — don't "clean up" or reformat it as if it were project code).
- **Metal** — scaffolded only. All ten backend `.m` files are ~9-line placeholders
  (`#if SPUDGPU_COMPILE_METAL_API` guard + comment). Blocked on Apple hardware access,
  not on unresolved design — the natives-header asymmetries above were already worked
  out. Don't start implementing without real hardware to validate against.

## Build system

- `GRAPHICS_BACKEND` is a single-choice CMake cache variable (`Vulkan` / `D3D12` /
  `Metal`) — there is no multi-backend build. The source list is assembled per-platform
  and per-backend *before* `add_library`, so only one backend's translation units ever
  compile in.
- `CMAKE_MSVC_RUNTIME_LIBRARY` is **not** set here — it must already be force-set by
  the parent `eqdev` root `CMakeLists.txt` before `add_subdirectory(spudlib)` runs (see
  `../CLAUDE.md`). If you're building `spudlib` standalone rather than through the
  workspace root, set it yourself before configuring, matching whatever ApricotFields/
  Erethal will use, or Debug builds will fail to link once combined.
- SDL3 integration is header-only glue (`spudgpu_sdl3.h`) — one inline
  `spudgpu_create_surface_from_sdl3` per backend (wraps `SDL_Vulkan_CreateSurface` on
  Vulkan, reads the raw `HWND` off SDL's window properties on D3D12). Don't grow this
  into a real SDL abstraction; it exists only to bridge surface creation.
- `.clang-format` is LLVM-based with tab indentation (`UseTab: ForIndentation`,
  `TabWidth`/`IndentWidth` 4), always-one-param-per-line, 160-column limit, and aligned
  consecutive assignments. Run it before committing.

## Rust bindings (`bindings/rust/`)

Independent Cargo workspace, **not** wired through CMake — `spudlib-sys`'s `build.rs`
compiles the Vulkan backend's `.c` files directly via the `cc` crate. It currently only
lists five of the ten Vulkan source files (context, swapchain, buffer, image, shader —
missing command, descriptors, renderpass, native, sync), and the safe `spudlib` wrapper
crate only covers `Instance`/`Device`/`Buffer`/`Image`/`ImageView`/`SwapChain`/
`CommandList` — no shader pipelines, descriptor sets, or fences/semaphores yet. This gap
is expected (built incrementally against what a minimal triangle-draw path needs), not
a regression to fix reflexively — extend it deliberately when a real Rust consumer
needs the next piece, matching the C API's existing shape rather than inventing a new
one.

## Working with in-progress changes

- If a fix/edit request touches a file that already has uncommitted work in
  progress (yours from an earlier session, or the user's own local changes),
  check `git diff` first. If the new fix's lines don't overlap that WIP, apply
  it directly to the working tree rather than isolating it on a separate
  branch/PR — that's extra ceremony for something that's going to land in the
  same working copy anyway. Only fall back to a standalone branch/PR when the
  tree is clean, or ask before proceeding if the fix's lines genuinely
  conflict with the WIP. Never discard or clobber the existing uncommitted
  work to make room for a fix.

## Known gaps (don't re-flag as surprises)

- SpudFiles has no Linux/macOS backend (Windows only).
- No automated test suite/CTest target exists — verification in this repo has been
  manual smoke-testing, not committed tests.
- Metal backend is unimplemented pending hardware access (see above).
