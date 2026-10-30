#include "tensor_lib/tensor.h"

#include <iostream>

int main()
{
    tensor_lib::Tensor tensor({2, 3}, 1.0F);
    tensor[0] = 2.0F;
    std::cout << "Tensor elements: " << tensor.size() << ", first value: " << tensor[0] << std::endl;
}
