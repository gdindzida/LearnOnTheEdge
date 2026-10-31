#pragma once

#include <array>
#include <cstddef>

namespace tensor_lib {

template <typename T, size_t... DIMS> class tensor_t {
public:
  static constexpr size_t gc_rank = sizeof...(DIMS);
  static constexpr size_t gc_size = (DIMS * ... * size_t{1});
  static constexpr std::array<size_t, gc_rank> gc_dimensions{DIMS...};

  using tensor_index_t = std::array<int, gc_rank>;
  using tensor_memory_t = std::array<T, gc_size>;

  explicit constexpr tensor_t() noexcept = default;

  constexpr T &operator()(tensor_index_t index) noexcept;

  constexpr tensor_memory_t &data() noexcept;

  void add(tensor_t &other) noexcept;
  void sub(tensor_t &other) noexcept;
  void mul(tensor_t &other) noexcept;

private:
  tensor_memory_t m_data;
};

} // namespace tensor_lib

