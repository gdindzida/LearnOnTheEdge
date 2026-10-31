#include "../src/tensor.cpp"

#include <gtest/gtest.h>

namespace {
using test_tensor_t = tensor_lib::tensor_t<int, 2, 3>;

TEST(Tensor, ExposesCompileTimeShape) {
  EXPECT_EQ(test_tensor_t::gc_rank, 2U);
  EXPECT_EQ(test_tensor_t::gc_size, 6U);
  EXPECT_EQ(test_tensor_t::gc_dimensions, (std::array<std::size_t, 2>{2, 3}));
}

TEST(Tensor, MapsTwoDimensionalIndicesInRowMajorOrder) {
  test_tensor_t tensor;
  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    tensor.data()[i] = static_cast<int>(i);
  }

  EXPECT_EQ(tensor({0, 0}), 0);
  EXPECT_EQ(tensor({0, 2}), 2);
  EXPECT_EQ(tensor({1, 0}), 3);
  EXPECT_EQ(tensor({1, 2}), 5);
}

TEST(Tensor, ProvidesMutableContiguousStorage) {
  test_tensor_t tensor;
  tensor.data()[4] = 42;

  EXPECT_EQ(tensor({1, 1}), 42);
}

TEST(Tensor, AddsElementwise) {
  test_tensor_t lhs;
  test_tensor_t rhs;
  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    lhs.data()[i] = static_cast<int>(i);
    rhs.data()[i] = 10;
  }

  lhs.add(rhs);

  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    EXPECT_EQ(lhs.data()[i], static_cast<int>(i) + 10);
  }
}

TEST(Tensor, SubtractsElementwise) {
  test_tensor_t lhs;
  test_tensor_t rhs;
  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    lhs.data()[i] = 20;
    rhs.data()[i] = static_cast<int>(i);
  }

  lhs.sub(rhs);

  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    EXPECT_EQ(lhs.data()[i], 20 - static_cast<int>(i));
  }
}

TEST(Tensor, MultipliesElementwise) {
  test_tensor_t lhs;
  test_tensor_t rhs;
  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    lhs.data()[i] = static_cast<int>(i + 1);
    rhs.data()[i] = 3;
  }

  lhs.mul(rhs);

  for (std::size_t i = 0; i < test_tensor_t::gc_size; ++i) {
    EXPECT_EQ(lhs.data()[i], static_cast<int>(i + 1) * 3);
  }
}
} // namespace
