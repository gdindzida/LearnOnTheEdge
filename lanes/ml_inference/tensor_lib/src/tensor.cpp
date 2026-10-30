#include "tensor_lib/tensor.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace tensor_lib {
namespace {

std::size_t element_count(const std::vector<std::size_t>& shape)
{
    if (std::any_of(shape.begin(), shape.end(), [](std::size_t extent) { return extent == 0; })) {
        throw std::invalid_argument("tensor extents must be non-zero");
    }

    return std::accumulate(shape.begin(), shape.end(), std::size_t{1}, std::multiplies<>{});
}

} // namespace

Tensor::Tensor(std::vector<std::size_t> shape, float initial_value)
    : shape_(std::move(shape)), data_(element_count(shape_), initial_value)
{
}

const std::vector<std::size_t>& Tensor::shape() const noexcept
{
    return shape_;
}

std::size_t Tensor::size() const noexcept
{
    return data_.size();
}

float& Tensor::operator[](std::size_t index)
{
    return data_.at(index);
}

const float& Tensor::operator[](std::size_t index) const
{
    return data_.at(index);
}
}
