#!/usr/bin/env bash
set -euo pipefail

bench_src=$(sed -n 's/^benchmark_SOURCE_DIR:STATIC=//p' build/linux-x64/CMakeCache.txt)
bench_lib=build/linux-x64/_deps/googlebenchmark-build/src
mkdir -p build/loop-alignment

compile() {
    g++ -std=c++17 -O3 -DNDEBUG -DBENCHMARK_STATIC_DEFINE \
        -Iinclude -I"$bench_src/include" benchmark/tensor_benchmark.cpp \
        "$bench_lib/libbenchmark_main.a" "$bench_lib/libbenchmark.a" \
        -pthread -lrt "$@"
}

compile -o build/loop-alignment/before
compile -falign-loops=32 -o build/loop-alignment/after
