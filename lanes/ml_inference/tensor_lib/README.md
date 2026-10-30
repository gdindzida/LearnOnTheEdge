# Static and shared tensor library

This project builds static and shared versions of the same minimal `tensor_lib`
library, plus one small executable for each version.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/tensor_lib_static_app
./build/tensor_lib_shared_app
```

The targets are `tensor_lib_static`, `tensor_lib_shared`, `tensor_lib_static_app`,
and `tensor_lib_shared_app`. The shared executable runs from the build directory;
when copying it elsewhere, copy `libtensor_lib_shared.so` too or make that
library available through the runtime library path.

## Linux x86-64 and AArch64

Follow the [local toolchain setup](../../../aarch64/README.md) first. Set
`REPO_ROOT` to the absolute repository path, even when building a copy of this
template elsewhere.

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-x64
./build/linux-x64/tensor_lib_static_app
./build/linux-x64/tensor_lib_shared_app

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Run AArch64 binaries on the Q6A. When deploying `tensor_lib_shared_app`, copy
`libtensor_lib_shared.so` beside it and run with `LD_LIBRARY_PATH="$PWD"`.
