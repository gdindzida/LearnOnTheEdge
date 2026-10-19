#include "blur_kernel.h"
#include <array>
#include <climits>
#include <iostream>

int main()
{
    // Hand-calculated answers check clamping, corners, edges, and the center.
    const std::array<uint8_t, 9> input{0, 9, 18, 27, 36, 45, 54, 63, 72};
    const std::array<uint8_t, 9> expected{12, 18, 24, 30, 36, 42, 48, 54, 60};
    std::array<uint8_t, 9> output{};
    if (blur_box3x3(input.data(), 9, output.data(), 9, 3, 3) || output != expected) {
        std::cerr << "3x3 golden image failed\n";
        return 1;
    }
    const uint8_t single = 255;
    uint8_t single_output = 0;
    if (blur_box3x3(&single, 1, &single_output, 1, 1, 1) || single_output != 255) {
        std::cerr << "1x1 image failed\n";
        return 1;
    }
    const std::array<uint8_t, 3> row{0, 0, 255};
    std::array<uint8_t, 3> row_output{};
    const std::array<uint8_t, 3> row_expected{0, 85, 170};
    if (blur_box3x3(row.data(), 3, row_output.data(), 3, 3, 1) || row_output != row_expected ||
        blur_box3x3(row.data(), 3, row_output.data(), 3, 1, 3) || row_output != row_expected) {
        std::cerr << "Single row/column failed\n";
        return 1;
    }
    std::array<uint8_t, 25> impulse{}, impulse_output{};
    impulse[12] = 255;
    if (blur_box3x3(impulse.data(), 25, impulse_output.data(), 25, 5, 5)) return 1;
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            const int value = (y >= 1 && y <= 3 && x >= 1 && x <= 3) ? 28 : 0;
            if (impulse_output[y * 5 + x] != value) {
                std::cerr << "Impulse/truncation failed\n";
                return 1;
            }
        }
    }
    if (blur_box3x3(nullptr, 9, output.data(), 9, 3, 3) == 0 ||
        blur_box3x3(input.data(), 9, nullptr, 9, 3, 3) == 0 ||
        blur_box3x3(input.data(), 8, output.data(), 9, 3, 3) == 0 ||
        blur_box3x3(input.data(), 9, output.data(), 8, 3, 3) == 0 ||
        blur_box3x3(input.data(), 9, output.data(), 9, 0, 3) == 0 ||
        blur_box3x3(input.data(), 9, output.data(), 9, -1, 3) == 0 ||
        blur_box3x3(input.data(), 9, output.data(), 9, INT_MAX, 2) == 0 ||
        blur_box3x3(output.data(), 9, output.data(), 9, 3, 3) == 0) {
        std::cerr << "Invalid buffer/dimension was accepted\n";
        return 1;
    }
    std::cout << "Blur golden images and invalid inputs passed.\n";
}
