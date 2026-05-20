// Pull in the pre-generated bindings
mod bindings;

// Re-export everything publicly so users of spudgpu-sys get it all
// from one flat namespace: spudgpu_sys::spudgpu_init, etc.
pub use bindings::*;