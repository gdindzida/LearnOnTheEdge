#include "../src/tensor.cpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace {
template <std::size_t Rows, std::size_t Cols>
using bench_tensor_t = tensor_lib::tensor_t<float, Rows, Cols>;

template <std::size_t Rows, std::size_t Cols, auto Operation, int LhsValue,
          int RhsValue>
// NOLINTNEXTLINE
void BM_TensorOperation(benchmark::State &state) {
  using tensor_t = bench_tensor_t<Rows, Cols>;
  auto lhs = std::make_unique<tensor_t>();
  auto rhs = std::make_unique<tensor_t>();
  for (std::size_t i = 0; i < tensor_t::gc_size; ++i) {
    lhs->data()[i] = LhsValue;
    rhs->data()[i] = RhsValue;
  }
  for (auto _ : state) {
    (lhs.get()->*Operation)(*rhs);
    benchmark::DoNotOptimize(lhs->data());
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() *
                          static_cast<std::int64_t>(tensor_t::gc_size));
}

template <std::size_t Rows, std::size_t Cols>
void BM_TensorAdd(benchmark::State &state) {
  BM_TensorOperation<Rows, Cols, &bench_tensor_t<Rows, Cols>::add, 1, 2>(state);
}
template <std::size_t Rows, std::size_t Cols>
void BM_TensorSub(benchmark::State &state) {
  BM_TensorOperation<Rows, Cols, &bench_tensor_t<Rows, Cols>::sub, 3, 1>(state);
}
template <std::size_t Rows, std::size_t Cols>
void BM_TensorMul(benchmark::State &state) {
  BM_TensorOperation<Rows, Cols, &bench_tensor_t<Rows, Cols>::mul, 2, 3>(state);
}

#define REGISTER_TENSOR_BENCHMARK(Operation, Rows, Cols)                       \
  BENCHMARK_TEMPLATE(BM_Tensor##Operation, Rows, Cols)                         \
      ->Name("Tensor" #Operation "/" #Rows "x" #Cols)

REGISTER_TENSOR_BENCHMARK(Add, 32, 32);
REGISTER_TENSOR_BENCHMARK(Add, 256, 256);
REGISTER_TENSOR_BENCHMARK(Add, 1024, 1024);
REGISTER_TENSOR_BENCHMARK(Add, 4096, 4096);
REGISTER_TENSOR_BENCHMARK(Sub, 32, 32);
REGISTER_TENSOR_BENCHMARK(Sub, 256, 256);
REGISTER_TENSOR_BENCHMARK(Sub, 1024, 1024);
REGISTER_TENSOR_BENCHMARK(Sub, 4096, 4096);
REGISTER_TENSOR_BENCHMARK(Mul, 32, 32);
REGISTER_TENSOR_BENCHMARK(Mul, 256, 256);
REGISTER_TENSOR_BENCHMARK(Mul, 1024, 1024);
REGISTER_TENSOR_BENCHMARK(Mul, 4096, 4096);
#undef REGISTER_TENSOR_BENCHMARK
} // namespace
