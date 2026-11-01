# Tensor library

## Navigation

- [Home](../../../README.md)
- [Parent: ML inference](../README.md)
- **Children**
  - [Check loop alignment](nuggets/function_aligment/README.md) — a side investigation of generated code and CPU performance.

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


## Benchmark stability settings

The benchmark executable deliberately uses the following Google Benchmark
settings for every case:

- **One-second warmup** lets the CPU reach a steady performance state before
  samples are recorded. This matters on the reference Ryzen 7 7700, where the
  runner reports that CPU frequency scaling is enabled.
- **A 0.5-second minimum measurement per sample** makes timer overhead and
  short-term scheduling variation insignificant relative to the work measured.
- **Three repetitions** provide a useful coefficient of variation (`cv`), so
  repeatability is visible instead of relying on one result.
- **Random interleaving** prevents registration order from biasing results; in
  particular, the first operation would otherwise be more likely to measure
  the CPU while it is still changing frequency.

These are implemented in the benchmark registration and entry point, rather
than requiring a long command line. They prioritize comparable measurements
over turnaround time: the 12 registered cases need at least about 30 seconds
(`12 × (1 s warmup + 3 × 0.5 s)`), plus setup and calibration.

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

## Benchmark results

These measurements were collected on 2026-11-01 from the Release build on the
reference Ryzen 7 7700 host, pinned to CPU 0. The host reported 32 KiB L1 data
caches (8), 1 MiB L2 caches (8), and a 32 MiB shared L3 cache.

The table shows CPU-time means across three repetitions; `cv` is the CPU-time
coefficient of variation. The benchmarks were warmed up and randomly
interleaved as described above. CPU scaling remained enabled, so the higher-CV
entries should be treated as less stable measurements.

The [loop-alignment walkthrough](nuggets/function_aligment/README.md) remains
side information: it documents a separate manual `-O3`/32-byte alignment
experiment, including disassembly and front-end counter analysis.

| Operation | Shape | Mean CPU time / iteration | Items / second | CPU `cv` |
| --- | ---: | ---: | ---: | ---: |
| Add | 32×32 | 49.0 ns | 20.90 G/s | 0.14% |
| Add | 256×256 | 3.906 μs | 16.78 G/s | 2.39% |
| Add | 1024×1024 | 72.877 μs | 14.42 G/s | 5.68% |
| Add | 4096×4096 | 4.088 ms | 4.10 G/s | 1.61% |
| Sub | 32×32 | 48.8 ns | 20.97 G/s | 0.09% |
| Sub | 256×256 | 4.440 μs | 14.95 G/s | 14.09% |
| Sub | 1024×1024 | 77.371 μs | 13.78 G/s | 16.42% |
| Sub | 4096×4096 | 3.998 ms | 4.20 G/s | 1.47% |
| Mul | 32×32 | 48.9 ns | 20.94 G/s | 0.01% |
| Mul | 256×256 | 3.882 μs | 16.89 G/s | 2.07% |
| Mul | 1024×1024 | 70.490 μs | 14.88 G/s | 1.01% |
| Mul | 4096×4096 | 4.101 ms | 4.09 G/s | 0.95% |

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

Follow the [local toolchain setup](../../../common/tools/aarch64/README.md) first. Set
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
