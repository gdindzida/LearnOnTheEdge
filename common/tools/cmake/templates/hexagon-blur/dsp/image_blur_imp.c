#include "image_blur.h"
#include "blur_kernel.h"
#include "AEEStdErr.h"

int image_blur_open(const char *uri, remote_handle64 *handle)
{
    (void)uri;
    if (!handle) return AEE_EBADPARM;
    /* This stateless service needs no per-client allocation. */
    *handle = 1;
    return AEE_SUCCESS;
}

int image_blur_close(remote_handle64 handle)
{
    (void)handle;
    return AEE_SUCCESS;
}

int image_blur_apply(remote_handle64 handle, const uint8 *input, int input_len,
                     uint8 *output, int output_len, int width, int height)
{
    (void)handle;
    return blur_box3x3(input, input_len, output, output_len, width, height) == 0
        ? AEE_SUCCESS : AEE_EBADPARM;
}
