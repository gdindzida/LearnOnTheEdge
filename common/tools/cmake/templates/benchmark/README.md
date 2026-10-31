# Google Benchmark

From this directory (Git and internet access are required on first configure):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/sum_benchmark
```

Use an optimized build for meaningful timings. The example measures summation
across several input sizes and keeps allocation outside the timed loop.
`benchmark::benchmark_main` supplies the entry point. To select a benchmark or
write JSON results, pass `--benchmark_filter=BM_Sum` or
`--benchmark_out=results.json --benchmark_out_format=json` to the executable.
For GCC and Clang-compatible compilers, Release adds `-O2`; other
configurations retain their usual compiler options.
For multi-configuration generators, build with `--config Release` and run
`build/Release/sum_benchmark` (with `.exe` on Windows).

## Linux x86-64 and AArch64

Follow the [local toolchain setup](../../../aarch64/README.md) first. Set `REPO_ROOT` to the
absolute repository path, even when building a copy of this template elsewhere.
From this template directory:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux-x64
./build/linux-x64/sum_benchmark

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Keep these build directories separate. Run ARM binaries on the Q6A.
To copy and run this example:

```sh
export Q6A_HOST=radxa@your-board-hostname
ssh "$Q6A_HOST" 'mkdir -p ~/cross-examples/benchmark'
scp build/linux-aarch64/sum_benchmark "$Q6A_HOST:cross-examples/benchmark/"
ssh "$Q6A_HOST" 'cd ~/cross-examples/benchmark && ./sum_benchmark'
```
