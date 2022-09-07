fn main() {
    println!("cargo:rustc-link-lib=spw");
    println!("cargo:rustc-link-search=c:\\tools\\spw");
    println!("cargo:rustc-link-search=d:\\Repo\\spw\\cpp\\build\\Release");
    // // Tell Cargo that if the given file changes, to rerun this build script.
    // println!("cargo:rerun-if-changed=src/hello.c");
    // // Use the `cc` crate to build a C file and statically link it.
    // cc::Build::new()
    //     .file("src/hello.c")
    //     .compile("hello");
}
