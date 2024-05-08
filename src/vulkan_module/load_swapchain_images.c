#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_load_swapchain_images(VkDevice device, VkSwapchainKHR swapchain, 
                                              unsigned int *image_count, VkImage **images, 
                                              VkImageView **image_views, VkFormat format)
{
    global_result = vkGetSwapchainImagesKHR(
        device, swapchain, image_count, 0x0);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SWAPCHAIN_IMAGES_ERROR;

    *images = realloc(*images, *image_count * sizeof(VkImage));

    if (!*images)
        return MALLOC_ERROR;

    global_result = vkGetSwapchainImagesKHR(
        device, swapchain, image_count, *images);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SWAPCHAIN_IMAGES_ERROR;

    *image_views = realloc(*image_views, *image_count * sizeof(VkImageView));

    if (!*image_views)
        return MALLOC_ERROR;

    (void) memset(*image_views, 0, *image_count * sizeof(VkImageView));

    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .format = format,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .components = { 
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    for (unsigned index = 0; index < *image_count; ++index)
    {
        view_create_info.image = (*images)[index];

        global_result = vkCreateImageView(
            device, &view_create_info, 0x0, *image_views + index);
        
        if (global_result != VK_SUCCESS)
            return VULKAN_IMAGE_VIEW_CREATION_ERROR;
    }

    return SUCCESS;
}
