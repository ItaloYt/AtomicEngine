#include "atomic/atom.h"
#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_recreate_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSwapchainKHR *swapchain, 
                                           VkSurfaceKHR surface, unsigned int *image_count, 
                                           VkSurfaceFormatKHR *format, VkExtent2D *extent, 
                                           VkPresentModeKHR *present_mode, 
                                           unsigned int graphic_queue_index, 
                                           unsigned int present_queue_index, 
                                           unsigned int *swapchain_image_count, VkImage **images, 
                                           VkImageView **image_views, VkRenderPass render_pass, VkFramebuffer **framebuffers)
{
    global_result = vkDeviceWaitIdle(device);
    
    if (global_result != VK_SUCCESS)
        return VULKAN_DEVICE_WAIT_IDLE_ERROR;

    for (unsigned index = 0; index < *swapchain_image_count; ++index)
    {
        vkDestroyImageView(device, (*image_views)[index], 0x0);
        vkDestroyFramebuffer(device, (*framebuffers)[index], 0x0);
    }

    vkDestroySwapchainKHR(device, *swapchain, 0x0);

    enum AtomicError error = vulkan_get_surface_information(
        physical_device, surface, extent, format, present_mode, image_count);

    if (error != SUCCESS) return error;

    error = vulkan_create_swapchain(
        device, swapchain, surface, *image_count, format,
        extent, *present_mode, graphic_queue_index, present_queue_index);

    if (error != SUCCESS) return error;

    error = vulkan_load_swapchain_images(
        device, *swapchain, swapchain_image_count,
        images, image_views, format->format);

    if (error != SUCCESS) return error;

    *framebuffers = realloc(*framebuffers, *swapchain_image_count * sizeof(VkFramebuffer));

    if (!*framebuffers)
        return MALLOC_ERROR;

    (void) memset(*framebuffers, 0, *swapchain_image_count * sizeof(VkImageView));

    error = vulkan_create_framebuffers(
        device, *image_views, *swapchain_image_count,
        render_pass, extent, *framebuffers);

    if (error != SUCCESS) return error;

    return SUCCESS;
}
