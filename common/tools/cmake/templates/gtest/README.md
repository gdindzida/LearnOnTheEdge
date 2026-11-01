# GoogleTest

## Navigation

- [Home](../../../../../README.md)
- [Parent: CMake templates](../README.md)

From this directory (Git and internet access are required on first configure):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

`calculator` exposes its include directory and C++ standard to consumers.
`calculator_tests` links GoogleTest's supplied main function, and
`gtest_discover_tests` registers individual test cases with CTest.
Configure with `-DBUILD_TESTING=OFF` to build only the library without downloading
GoogleTest. For multi-configuration generators, build with `--config Debug` and
pass `-C Debug` to CTest.

For GCC and Clang-compatible compilers, a Release configuration adds `-O2`;
Debug and other configurations retain their usual compiler options.

## Linux x86-64 and AArch64

Follow the [local toolchain setup](../../../aarch64/README.md) first. Set `REPO_ROOT` to the
absolute repository path, even when building a copy of this template elsewhere.
From this template directory:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-x64
ctest --test-dir build/linux-x64 --output-on-failure

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Keep these build directories separate. Run ARM binaries on the Q6A.
To copy and run this example:

```sh
export Q6A_HOST=radxa@your-board-hostname
ssh "$Q6A_HOST" 'mkdir -p ~/cross-examples/gtest'
scp build/linux-aarch64/calculator_tests "$Q6A_HOST:cross-examples/gtest/"
ssh "$Q6A_HOST" 'cd ~/cross-examples/gtest && ./calculator_tests'
```

Cross builds compile the tests but skip host CTest registration and test
discovery. Run `calculator_tests` directly on the board; native CTest still
discovers individual tests. `BUILD_TESTING=OFF` works for either architecture.
