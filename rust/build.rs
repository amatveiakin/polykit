fn main() {
    cc::Build::new()
        .cpp(true)
        .opt_level(3)
        .define("NDEBUG", "1")
        .file("linbox_bindings/bindings.cpp")
        .compile("bindings.a");
    println!("cargo:rustc-link-lib=gmp");
    println!("cargo:rustc-link-lib=givaro");
    println!("cargo:rustc-link-lib=ntl");
    println!("cargo:rerun-if-changed=linbox_bindings");
}
