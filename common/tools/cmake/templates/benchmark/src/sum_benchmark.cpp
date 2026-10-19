#include <benchmark/benchmark.h>

#include <cstdint>
#include <numeric>
#include <vector>

static void BM_Sum(benchmark::State& state)
{
    // Allocate and initialize outside the measured loop.
    std::vector<int> values(static_cast<std::size_t>(state.range(0)), 1);
    for (auto _ : state) {
        benchmark::DoNotOptimize(values.data());
        const auto sum = std::accumulate(values.begin(), values.end(), std::int64_t{0});
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_Sum)->RangeMultiplier(4)->Range(64, 65536);
