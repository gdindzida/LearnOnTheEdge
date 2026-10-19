#include "blur_kernel.h"
#include <limits.h>

static int clamp(int value, int limit)
{
    if (value < 0) return 0;
    if (value >= limit) return limit - 1;
    return value;
}

int blur_box3x3(const uint8_t *input, int input_len,
                uint8_t *output, int output_len, int width, int height)
{
    if (!input || !output || input == output || width <= 0 || height <= 0 ||
        width > INT_MAX / height) {
        return -1;
    }
    const int pixels = width * height;
    if (input_len < pixels || output_len < pixels) return -1;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned int sum = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                const int row = clamp(y + dy, height) * width;
                for (int dx = -1; dx <= 1; ++dx) {
                    sum += input[row + clamp(x + dx, width)];
                }
            }
            output[y * width + x] = (uint8_t)(sum / 9);
        }
    }
    return 0;
}
