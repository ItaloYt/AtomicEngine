#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_swapchain(VkDevice device, VkSwapchainKHR *swapchain, 
                                         VkSurfaceKHR surface, unsigned image_count, 
                                         const VkSurfaceFormatKHR *format,
                                         const VkExtent2D *extent,
                                         VkPresentModeKHR present_mode,
                                         unsigned graphic_queue_index, 
                                         unsigned present_queue_index)
{
    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = format->format,
        .imageColorSpace = format->colorSpace,
        .imageExtent = *extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = 1
    };

    if (graphic_queue_index != present_queue_index)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = (unsigned[]) { graphic_queue_index, present_queue_index };
    }
    else create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    global_result = vkCreateSwapchainKHR(device, &create_info, 0x0, swapchain);

    if (global_result != VK_SUCCESS)
    	return VULKAN_SWAPCHAIN_CREATION_ERROR;

    return SUCCESS;
}
