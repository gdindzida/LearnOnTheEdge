#ifndef BLUR_KERNEL_H
#define BLUR_KERNEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 3x3 box blur of a row-major grayscale image. Clamp coordinates at edges;
 * divide the nine-pixel sum by 9 using integer truncation. Input/output must
 * be separate, non-overlapping buffers. Return 0 on success, -1 on bad input.
 */
int blur_box3x3(const uint8_t *input, int input_len,
                uint8_t *output, int output_len, int width, int height);

#ifdef __cplusplus
}
#endif
#endif
