#include "../src/tensor.cpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

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
      ->Name("Tensor" #Operation "/" #Rows "x" #Cols)                          \
      ->MinWarmUpTime(1.0)                                                     \
      ->MinTime(0.5)                                                           \
      ->Repetitions(3)

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

namespace {
bool has_random_interleaving_argument(int argc, char *const argv[]) {
  constexpr char c_flag[] = "--benchmark_enable_random_interleaving";
  constexpr std::size_t c_flagLength = sizeof(c_flag) - 1;

  for (int index = 1; index < argc; ++index) {
    if (std::strncmp(argv[index], c_flag, c_flagLength) == 0 &&
        (argv[index][c_flagLength] == '\0' ||
         argv[index][c_flagLength] == '=')) {
      return true;
    }
  }
  return false;
}
} // namespace

int main(int argc, char **argv) {
  char defaultProgramName[] = "tensor_lib_benchmark";
  char *defaultArgv[] = {defaultProgramName};
  if (argv == nullptr) {
    argc = 1;
    argv = defaultArgv;
  }

  char randomInterleaving[] = "--benchmark_enable_random_interleaving=true";
  std::vector<char *> arguments(argv, argv + argc);
  if (!has_random_interleaving_argument(argc, argv)) {
    arguments.push_back(randomInterleaving);
  }

  int parsedArgc = static_cast<int>(arguments.size());
  benchmark::Initialize(&parsedArgc, arguments.data());
  if (benchmark::ReportUnrecognizedArguments(parsedArgc, arguments.data())) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
