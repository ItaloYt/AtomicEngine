#include "atomic/camera.h"
#include "atomic/util.h"
#include <vulkan/vulkan_core.h>

#define INV_SQRT_3 0.577350269f // 1 / sqrt(3)

void camera_update_projection(struct Camera *camera, const VkExtent2D *extent)
{
    camera->projection[0] = (float) extent->height / extent->width * INV_SQRT_3;
    camera->projection[5] = INV_SQRT_3;
    camera->projection[10] = camera->far / (camera->far - camera->near);
    camera->projection[11] = 1;
    camera->projection[14] = camera->far * camera->near / (camera->far - camera->near); //100 * 0.3 / 99.7
    camera->projection[15] = 1;
}
