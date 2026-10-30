#pragma once

#include <cstddef>
#include <vector>

namespace tensor_lib {

class Tensor {
public:
    explicit Tensor(std::vector<std::size_t> shape, float initial_value = 0.0F);

    const std::vector<std::size_t>& shape() const noexcept;
    std::size_t size() const noexcept;

    float& operator[](std::size_t index);
    const float& operator[](std::size_t index) const;

private:
    std::vector<std::size_t> shape_;
    std::vector<float> data_;
};

}
