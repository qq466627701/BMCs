SUMMARY = "LLVM compiler framework (packaged with rust)"
LICENSE ?= "Apache-2.0-with-LLVM-exception"
HOMEPAGE = "http://www.rust-lang.org"

# check src/llvm-project/llvm/CMakeLists.txt for llvm version in use
#
LLVM_RELEASE = "15.0.6"

require rust-source.inc

SRC_URI += "file://0002-llvm-allow-env-override-of-exe-path.patch;striplevel=2 \
            file://0001-AsmMatcherEmitter-sort-ClassInfo-lists-by-name-as-we.patch;striplevel=2 \
	    file://0003-llvm-fix-include-benchmarks.patch;striplevel=2 \
	    file://0035-cmake-Enable-64bit-off_t-on-32bit-glibc-systems.patch;striplevel=2"

S = "${RUSTSRC}/src/llvm-project/llvm"

LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=8a15a0759ef07f2682d2ba4b893c9afe"

inherit cmake python3native

DEPENDS += "ninja-native rust-llvm-native"

ARM_INSTRUCTION_SET:armv5 = "arm"
ARM_INSTRUCTION_SET:armv4t = "arm"

# rustc_llvm with debug info is not recognized as a valid crate that's
# generated by rust-llvm-native.
CFLAGS:remove = "-g"
CXXFLAGS:remove = "-g"

LLVM_DIR = "llvm${LLVM_RELEASE}"

EXTRA_OECMAKE = " \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_TARGETS_TO_BUILD='ARM;AArch64;Mips;PowerPC;RISCV;X86' \
    -DLLVM_BUILD_DOCS=OFF \
    -DLLVM_ENABLE_TERMINFO=OFF \
    -DLLVM_ENABLE_ZLIB=OFF \
    -DLLVM_ENABLE_ZSTD=OFF \
    -DLLVM_ENABLE_LIBXML2=OFF \
    -DLLVM_ENABLE_FFI=OFF \
    -DLLVM_INSTALL_UTILS=ON \
    -DLLVM_BUILD_EXAMPLES=OFF \
    -DLLVM_INCLUDE_EXAMPLES=OFF \
    -DLLVM_BUILD_TESTS=OFF \
    -DLLVM_INCLUDE_TESTS=OFF \
    -DLLVM_TARGET_ARCH=${TARGET_ARCH} \
    -DCMAKE_INSTALL_PREFIX:PATH=${libdir}/llvm-rust \
"
EXTRA_OECMAKE:append:class-target = "\
    -DCMAKE_CROSSCOMPILING:BOOL=ON \
    -DLLVM_BUILD_TOOLS=OFF \
    -DLLVM_TABLEGEN=${STAGING_LIBDIR_NATIVE}/llvm-rust/bin/llvm-tblgen \
    -DLLVM_CONFIG_PATH=${STAGING_LIBDIR_NATIVE}/llvm-rust/bin/llvm-config \
"

EXTRA_OECMAKE:append:class-nativesdk = "\
    -DCMAKE_CROSSCOMPILING:BOOL=ON \
    -DLLVM_BUILD_TOOLS=OFF \
    -DLLVM_TABLEGEN=${STAGING_LIBDIR_NATIVE}/llvm-rust/bin/llvm-tblgen \
    -DLLVM_CONFIG_PATH=${STAGING_LIBDIR_NATIVE}/llvm-rust/bin/llvm-config \
"

# The debug symbols are huge here (>2GB) so suppress them since they
# provide almost no value. If you really need them then override this
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

export YOCTO_ALTERNATE_EXE_PATH = "${STAGING_LIBDIR}/llvm-rust/bin/llvm-config"

do_install:append () {
    # we don't need any of this stuff to build Rust
    rm -rf "${D}/usr/lib/cmake"
}

PACKAGES =+ "${PN}-bugpointpasses ${PN}-llvmhello ${PN}-liblto"

# Add the extra locations to avoid the complaints about unpackaged files
FILES:${PN}-bugpointpasses = "${libdir}/llvm-rust/lib/BugpointPasses.so"
FILES:${PN}-llvmhello = "${libdir}/llvm-rust/lib/LLVMHello.so"
FILES:${PN}-liblto = "${libdir}/llvm-rust/lib/libLTO.so.*"
FILES:${PN}-staticdev =+ "${libdir}/llvm-rust/*/*.a"
FILES:${PN} += "${libdir}/libLLVM*.so.* ${libdir}/llvm-rust/lib/*.so.* ${libdir}/llvm-rust/bin"
FILES:${PN}-dev += "${datadir}/llvm ${libdir}/llvm-rust/lib/*.so ${libdir}/llvm-rust/include ${libdir}/llvm-rust/share ${libdir}/llvm-rust/lib/cmake"

BBCLASSEXTEND = "native nativesdk"
