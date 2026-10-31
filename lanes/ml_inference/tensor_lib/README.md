# Tensor library

This project builds static and shared versions of `tensor_lib`, a GoogleTest
suite, and Google Benchmark cases for elementwise addition, subtraction, and
multiplication.

From this directory, configure and build all targets with:

```sh
cmake -S . -B build/linux-x64 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux-x64
```

CMake fetches GoogleTest and Google Benchmark during configuration. Run the
unit tests through CTest:

```sh
ctest --test-dir build/linux-x64 --output-on-failure
```

Or run the test executable directly:

```sh
./build/linux-x64/gtest/tensor_lib_tests
```

With GCC or a Clang-compatible compiler, Release builds add `-O2` for C++
compilation after CMake's default Release flags. With GCC specifically, the
benchmark target also adds `-falign-loops=32` in Release builds. Other build
types retain their usual compiler options.

Build the benchmark target explicitly with:

```sh
cmake --build build/linux-x64 --target tensor_lib_benchmark
```

Run all arithmetic benchmarks with:

```sh
./build/linux-x64/benchmark/tensor_lib_benchmark
```

Each operation is benchmarked at 32×32, 256×256, 1024×1024, and 4096×4096.
These sizes place the two tensor operands at different points in the cache
hierarchy. The reference machine reported 32 KiB L1 data caches, 1 MiB L2
caches, and a 32 MiB shared L3 cache. Cache sizes vary by processor; these
comparisons explain the workload choices and are not a promise that every CPU
has the same cache boundaries.

| Shape | One tensor (`float`) | Two operands | Approximate cache relationship |
| --- | ---: | ---: | --- |
| 32×32 | 4 KiB | 8 KiB | Fits in a 32 KiB L1 data cache, so this is the cache-resident baseline. |
| 256×256 | 256 KiB | 512 KiB | Both operands fit within a 1 MiB L2, leaving room for additional cache state. |
| 1024×1024 | 4 MiB | 8 MiB | Both operands exceed private L2 by a wider margin while fitting in the 32 MiB shared L3. |
| 4096×4096 | 64 MiB | 128 MiB | The operands exceed the shared L3, making this the large working-set case that exercises main-memory traffic. |

Each operation updates the left tensor in place, so the CPU also reads and
writes that tensor during each iteration. The table compares allocated operand
capacity; actual cache traffic depends on the operation, cache policy, and
processor. Together, the cases show the transition from L1-resident work,
through L2/L3-sized working sets, to a working set larger than L3.

## First benchmark results

These are historical measurements, recorded before this project standardized
Release builds on `-O2` and before the GCC Release benchmark target enabled
32-byte loop alignment. See the [loop-alignment walkthrough](nuggets/function_aligment/README.md)
for the separate manual `-O3`/32-byte before-and-after experiment, function
addresses and sizes, disassembly, and cache-line calculations.

The following is the first recorded result set from a short smoke run on the
reference machine after selecting the current four sizes. It used Release
mode and `--benchmark_min_time=0.001s`. Times are CPU time per iteration; results are
machine-specific, and the brief run is intended as an initial baseline rather
than a stable comparison. The host reported 32 KiB L1 data caches (8), 1 MiB
L2 caches (8), and a 32 MiB L3 cache (1).

| Operation | Shape | CPU time / iteration | Items / second |
| --- | ---: | ---: | ---: |
| Add | 32×32 | 60.2 ns | 17.00 G/s |
| Add | 256×256 | 4.816 μs | 13.61 G/s |
| Add | 1024×1024 | 86.007 μs | 12.19 G/s |
| Add | 4096×4096 | 4.579 ms | 3.66 G/s |
| Sub | 32×32 | 62.7 ns | 16.34 G/s |
| Sub | 256×256 | 4.685 μs | 13.99 G/s |
| Sub | 1024×1024 | 93.673 μs | 11.19 G/s |
| Sub | 4096×4096 | 5.091 ms | 3.30 G/s |
| Mul | 32×32 | 92.6 ns | 11.06 G/s |
| Mul | 256×256 | 5.861 μs | 11.18 G/s |
| Mul | 1024×1024 | 85.134 μs | 12.32 G/s |
| Mul | 4096×4096 | 4.743 ms | 3.54 G/s |

To do a short smoke run across all 12 cases, use:

```sh
./build/linux-x64/benchmark/tensor_lib_benchmark --benchmark_min_time=0.001s
```

To run a single operation, pass a Google Benchmark filter, for example:

```sh
./build/linux-x64/benchmark/tensor_lib_benchmark --benchmark_filter="TensorAdd/.*"
```

Tests and benchmarks are enabled by default. To configure only the library,
turn either feature off:

```sh
cmake -S . -B build/library-only -DBUILD_TESTING=OFF -DTENSOR_LIB_BUILD_BENCHMARKS=OFF
```

## AArch64

Follow the [local toolchain setup](../../../aarch64/README.md) first. Set
`REPO_ROOT` to the absolute repository path, then configure and build:

```sh
export REPO_ROOT=/absolute/path/to/LearnOnTheEdge

cmake -S . -B build/linux-aarch64 -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/common/tools/aarch64/toolchain.cmake"
cmake --build build/linux-aarch64
```

Run the AArch64 test and benchmark executables on the Q6A:

```sh
./build/linux-aarch64/gtest/tensor_lib_tests
./build/linux-aarch64/benchmark/tensor_lib_benchmark
```
