#include "tensor.h"

namespace tensor_lib {
namespace {} // namespace

template <typename T, std::size_t... DIMS>
constexpr T &
tensor_t<T, DIMS...>::operator()(tensor_index_t index) noexcept {
  std::size_t linearIndex = 0;

  for (std::size_t dimension = 0; dimension < gc_rank; ++dimension) {
    linearIndex = linearIndex * gc_dimensions[dimension] + index[dimension];
  }

  return m_data[linearIndex];
}

template <typename T, std::size_t... DIMS>
constexpr typename tensor_t<T, DIMS...>::tensor_memory_t &
tensor_t<T, DIMS...>::data() noexcept {
  return m_data;
}

template <typename T, std::size_t... DIMS>
void tensor_t<T, DIMS...>::add(tensor_t &other) noexcept {
  auto &otherData = other.data();

  for (std::size_t linearIndex = 0; linearIndex < gc_size; ++linearIndex) {
    m_data[linearIndex] += otherData[linearIndex];
  }
}

template <typename T, std::size_t... DIMS>
void tensor_t<T, DIMS...>::sub(tensor_t &other) noexcept {
  auto &otherData = other.data();

  for (std::size_t linearIndex = 0; linearIndex < gc_size; ++linearIndex) {
    m_data[linearIndex] -= otherData[linearIndex];
  }
}

template <typename T, std::size_t... DIMS>
void tensor_t<T, DIMS...>::mul(tensor_t &other) noexcept {
  auto &otherData = other.data();

  for (std::size_t linearIndex = 0; linearIndex < gc_size; ++linearIndex) {
    m_data[linearIndex] *= otherData[linearIndex];
  }
}

} // namespace tensor_lib
