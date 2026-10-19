# AArch64 Linux cross-compilation

This setup targets ordinary C/C++ Linux applications on the Radxa Dragon Q6A
running Ubuntu 24.04 (Noble). The build host must be Linux x86-64. Native x86-64
builds continue to use the host compiler.

## Install

Install host prerequisites on Ubuntu:

```sh
sudo apt update
sudo apt install build-essential cmake ninja-build git curl xz-utils
```

Define the absolute repository location, then run setup from any directory:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge
bash "$REPO_ROOT/common/tools/aarch64/setup.sh"
```

Setup downloads Arm GNU Toolchain **13.3.Rel1**, verifies its pinned SHA-256,
then extracts it alongside this README. It needs no sudo and does not alter PATH.
Repeated runs reuse a completed installation after checking its marker, compiler
version, target, and sysroot. Archives and incomplete extractions are cleaned up;
the extracted bundle is ignored by Git. To reinstall, remove only the versioned
bundle directory and rerun setup.

- Bundle: `arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz`
- Compiler prefix: `aarch64-none-linux-gnu-` (Linux with glibc)
- SHA-256: `322f0b4482fc0d9fa0bb468134841f08d8c554c54ff5aa29a13a7a24bf7e1eb5`
- [Official download](https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz)
- [Published checksum](https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz.sha256asc)
- [Release notes](https://documentation-service.arm.com/static/66bb3acb882fec713ef48f84)

The bundle includes GCC 13.3 and a glibc 2.38 sysroot. The current examples use
only standard system libraries and dependencies built from source. Board-specific
libraries require a separate matching target sysroot and are outside this setup.
Do not add host `/usr/include` or `/usr/lib` directories to cross builds.

## Build

From any template (or a copied template), after defining `REPO_ROOT` above:

```sh
cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-x64

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Use `Release` for benchmarks. Always keep architectures in separate build
directories. If moving the repository or changing the compiler, delete the
corresponding build directory and reconfigure. The toolchain uses absolute paths
to the local compiler, including inside CMake's compiler checks. It does not use
the system's `aarch64-linux-gnu-g++`.

## Run on the Q6A

On the board, confirm the OS and architecture:

```sh
cat /etc/os-release
uname -m
getconf GNU_LIBC_VERSION
```

Expect Noble and `aarch64`. These builds use dynamic system runtimes; verify them
on the board rather than assuming that a successful cross build guarantees ABI
compatibility. `ldd ./basic_app` on the board should resolve every dependency
without GLIBC or GLIBCXX version errors. Keep the board's libc6 and libstdc++6
packages available; do not replace its libc with files from the toolchain.

For example, from the basic template directory:

```sh
export Q6A_HOST=radxa@your-board-hostname
ssh "$Q6A_HOST" 'mkdir -p ~/cross-examples/basic'
scp build/linux-aarch64/basic_app "$Q6A_HOST:cross-examples/basic/"
ssh "$Q6A_HOST" 'cd ~/cross-examples/basic && ./basic_app'
```

Each template README gives its executable and any extra library-copy steps.
Run GoogleTest directly on the board; cross builds deliberately do not register
host CTest tests. Native builds continue to use CTest normally. No QEMU or
automatic SSH deployment is configured.

Inspect an ARM binary on the host with `file` or the bundled
`bin/aarch64-none-linux-gnu-readelf -h`: its machine must be AArch64. Benchmark
results should be collected on the actual board using a Release build.
