use std::path::PathBuf;

fn main() {
    let manifest_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));

    // Walk up from bindings/rust/spudgpu-sys/ to the repo root
    // Adjust this if your layout differs
    let root = manifest_dir
        .join("../../..")
        .canonicalize()
        .expect("Could not resolve repo root");

    // ----------------------------------------------------------------
    // 1. Compile the SpudGPU C sources into a static library
    // ----------------------------------------------------------------
    let mut build = cc::Build::new();

    build
        .file(root.join("spudgpuvulkancontext.c"))
        .file(root.join("spudgpuvulkanbuffer.c"))
        .file(root.join("spudgpuvulkanimage.c"))
        .include(&root)
        .flag_if_supported("-std=c11");

    if cfg!(feature = "vulkan") {
        build.define("SPUDGPU_COMPILE_VULKAN_API", "1");
    }

    if cfg!(feature = "metal") {
        build.define("SPUDGPU_COMPILE_METAL_API", "1");
    }

    build.compile("spudgpu");

    // ----------------------------------------------------------------
    // 2. Link platform Vulkan library
    // ----------------------------------------------------------------
    #[cfg(feature = "vulkan")]
    {
        println!("cargo:rustc-link-lib=vulkan");
    }

    // ----------------------------------------------------------------
    // 3. Tell Cargo to re-run this script if C sources change
    // ----------------------------------------------------------------
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}", root.join("spudgpu.h").display());
    println!("cargo:rerun-if-changed={}", root.join("spudgpuvulkan.h").display());
    println!("cargo:rerun-if-changed={}", root.join("spudgpuvulkancontext.c").display());
    println!("cargo:rerun-if-changed={}", root.join("spudgpuvulkanbuffer.c").display());
    println!("cargo:rerun-if-changed={}", root.join("spudgpuvulkanimage.c").display());
}