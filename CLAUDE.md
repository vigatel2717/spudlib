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
- **Dynamic rendering, not render-pass objects.** The renderpass concept was removed
  in favor of `spudgpu_cmd_begin_rendering`/`cmd_end_rendering` (commit `e90f921`).
  Don't reintroduce a `VkRenderPass`/`VkFramebuffer`-style object into the public API —
  D3D12's backend already emulates this begin/end shape on top of its native
  render-target-view binding, not the other way around.
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
  - Fences and semaphores both map to `id<MTLSharedEvent>`, deliberately not
    `MTLFence` (a plain fence can't be waited on from the CPU or across command
    buffers)

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
