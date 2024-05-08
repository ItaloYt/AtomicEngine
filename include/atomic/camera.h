#ifndef ATOMIC_CAMERA_HEADER
#define ATOMIC_CAMERA_HEADER

#include "atomic/types.h"
#include "atomic/util.h"
#include <vulkan/vulkan_core.h>

struct Camera {
    mat4 projection;
    
    union {
        mat4 view;

        struct {
            vec4 view_right;
            vec4 view_up;
            vec4 view_forward;
            vec4 position;
        };
    };

    quat rotation;

    float far;
    float near;
};

void camera_update_projection(struct Camera *camera, const VkExtent2D *extent);

#endif
