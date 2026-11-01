# CMake templates

## Navigation

- [Home](../../../../README.md)
- [Parent: CMake](../README.md)
- **Children**
  - [Basic executable](basic/README.md) — a C++ executable.
  - [GoogleTest](gtest/README.md) — a library with GoogleTest and CTest.
  - [Google Benchmark](benchmark/README.md) — a microbenchmark.
  - [Library](library/README.md) — static and shared libraries with example executables.
  - [Hexagon blur](hexagon-blur/README.md) — a CPU/DSP image-blur application.

Copy an example directory into a new project and rename its project and targets.
Each example is independent; there is no top-level build combining them.

The general-purpose examples require CMake 3.20 or newer and a C++17 compiler. GoogleTest and
Google Benchmark require Git and network access on the first configure; their
versions are pinned in the respective CMakeLists.txt files. No global dependency
installation is needed. Build commands are in each example's README.

For GCC and Clang-compatible compilers, the templates add `-O2` to C and C++
Release builds. No build type is selected automatically, and other build types
keep their usual compiler options.

## Linux build targets

The general-purpose templates support native Linux x86-64 and AArch64 Linux cross builds.
Install the [local AArch64 toolchain](../../aarch64/README.md), then use each
example's explicit CMake commands. Native builds use the host compiler; cross
builds use the pinned bundle and its sysroot. Keep outputs in `build/linux-x64`
and `build/linux-aarch64`. ARM executables run on the Q6A, including tests and
benchmarks. Templates remain independent and can be copied outside this repo by
passing the absolute toolchain file path.

The `hexagon-blur` template additionally uses the Hexagon SDK toolchains and FastRPC; its build and deployment instructions are in [`hexagon-blur/README.md`](hexagon-blur/README.md).
