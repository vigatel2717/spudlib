// swift-tools-version:5.9
import PackageDescription

let package = Package(
    name: "SpudLib",
    platforms: [
        .macOS(.v14)
    ],
    products: [
        .library(name: "SpudLib", targets: ["SpudLib"])
    ],
    targets: [
        // Raw C-ABI surface. Sources here are symlinks into ../../include and
        // ../../src - spudlib has one source of truth (this package doesn't
        // vendor a copy), same principle as bindings/rust/spudlib-sys/build.rs
        // compiling the real .c files directly rather than duplicating them.
        //
        // Only spudcore.c + the Metal context/buffer backend are wired in today,
        // and only spudcore.h/spudgpu.h are exposed publicly, matching what
        // SpudLib/SpudGPU.swift actually calls - extend deliberately (symlink
        // the next source/header in) when a real consumer needs the next piece,
        // rather than pulling in the whole spudgpu backend or every header
        // (some of which, e.g. spudfiles.h, assume a C23 `bool` keyword that
        // isn't available under the C standard Swift uses to parse this module)
        // up front.
        .target(
            name: "CSpudLib",
            path: "Sources/CSpudLib",
            sources: [
                "spudcore.c",
                "spudgpumetalcontext.m",
                "spudgpumetalbuffer.m",
            ],
            publicHeadersPath: "include",
            cSettings: [
                .headerSearchPath("include"),
                .define("SPUDGPU_COMPILE_METAL_API", to: "1"),
                // The Metal backend uses manual retain/release (matching the
                // CMake build, which compiles it as plain, non-ARC Objective-C) -
                // SwiftPM defaults .m files to ARC, which rejects explicit
                // release/retain calls, so opt back out here.
                .unsafeFlags(["-fno-objc-arc"]),
            ],
            linkerSettings: [
                .linkedFramework("Metal"),
                .linkedFramework("QuartzCore"),
                .linkedFramework("Foundation"),
            ]
        ),
        .target(
            name: "SpudLib",
            dependencies: ["CSpudLib"],
            path: "Sources/SpudLib"
        ),
    ]
)
