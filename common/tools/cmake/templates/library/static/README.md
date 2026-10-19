# Static library

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/greeting_app
```

`add_library(greeting STATIC ...)` explicitly selects the library type.
The executable links `Greeting::greeting` and inherits its public include paths
and C++17 requirement. For multi-configuration generators, build with
`--config Debug` and run `build/Debug/greeting_app` (with `.exe` on Windows).

The resulting archive is incorporated into the executable during linking.

## Linux x86-64 and AArch64

Follow the [local toolchain setup](../../../../aarch64/README.md) first. Set `REPO_ROOT` to the
absolute repository path, even when building a copy of this template elsewhere.
From this template directory:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-x64
./build/linux-x64/greeting_app

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Keep these build directories separate. Run ARM binaries on the Q6A.
To copy and run this example:

```sh
export Q6A_HOST=radxa@your-board-hostname
ssh "$Q6A_HOST" 'mkdir -p ~/cross-examples/library/static'
scp build/linux-aarch64/greeting_app "$Q6A_HOST:cross-examples/library/static/"
ssh "$Q6A_HOST" 'cd ~/cross-examples/library/static && ./greeting_app'
```
