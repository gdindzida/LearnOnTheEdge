# Basic executable

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/basic_app
```

Add source files to `add_executable` and set requirements on the target with
`target_compile_features`, `target_include_directories`, and `target_link_libraries`.
For multi-configuration generators, build with `--config Debug` and run the
executable from `build/Debug/` (with `.exe` on Windows).

## Linux x86-64 and AArch64

Follow the [local toolchain setup](../../../aarch64/README.md) first. Set `REPO_ROOT` to the
absolute repository path, even when building a copy of this template elsewhere.
From this template directory:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-x64
./build/linux-x64/basic_app

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Keep these build directories separate. Run ARM binaries on the Q6A.
To copy and run this example:

```sh
export Q6A_HOST=radxa@your-board-hostname
ssh "$Q6A_HOST" 'mkdir -p ~/cross-examples/basic'
scp build/linux-aarch64/basic_app "$Q6A_HOST:cross-examples/basic/"
ssh "$Q6A_HOST" 'cd ~/cross-examples/basic && ./basic_app'
```
