#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include "atomic/spirv.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

VkResult global_result = VK_SUCCESS;

enum AtomicError vulkan_initialize(struct VulkanContext *context)
{
    register enum AtomicError error = vulkan_create_instance(&context->instance);

    if (error != SUCCESS) return error;

    error = vulkan_retrieve_window_surface(&context->surface);

    if (error != SUCCESS) return error;

    VkPhysicalDevice *physical_device_list = 0x0;
    unsigned physical_device_list_length = 0;

    error = vulkan_load_physical_devices(
        context->instance,
        &physical_device_list,
        &physical_device_list_length);

    if (error != SUCCESS) return error;

    context->physical_device = vulkan_select_physical_device(
        physical_device_list,
        physical_device_list_length);

    free(physical_device_list);

    error = vulkan_load_queue_families(
        context->physical_device,
        context->surface,
        &context->graphic_index,
        &context->present_index);

    if (error != SUCCESS) return error;

    if (context->graphic_index == context->present_index)
    {   
        error = vulkan_create_device(
            context->physical_device,
            &context->device,
            (unsigned[]) { context->graphic_index },
            (VkQueue *[]) { &context->graphic_queue },
            1);

        context->present_queue = context->graphic_queue;
    }
    else
    {
        error = vulkan_create_device(
            context->physical_device,
            &context->device, 
            (unsigned[]) { context->graphic_index, context->present_index },
            (VkQueue *[]) { &context->graphic_queue, &context->present_queue },
            2);
    }

    if (error != SUCCESS) return error;

    error = vulkan_get_surface_information(
        context->physical_device,
        context->surface,
        &context->extent,
        &context->format,
        &context->present_mode,
        &context->image_count);

    if (error != SUCCESS) return error;

    error = vulkan_create_swapchain(
        context->device,
        &context->swapchain,
        context->surface,
        context->image_count,
        &context->format, 
        &context->extent,
        context->present_mode,
        context->graphic_index,
        context->present_index);

    if (error != SUCCESS) return error;

    error = vulkan_load_swapchain_images(
        context->device,
        context->swapchain,
        &context->swapchain_image_count, 
        &context->images, 
        &context->image_views, 
        context->format.format);

    if (error != SUCCESS) return error;

    error = vulkan_create_render_pass(context->device, context->format.format, &context->render_pass);

    if (error != SUCCESS) return error;

    context->framebuffers = calloc(context->swapchain_image_count, sizeof(VkFramebuffer));

    if (!context->framebuffers)
        return MALLOC_ERROR;

    error = vulkan_create_framebuffers(
        context->device, context->image_views,
        context->swapchain_image_count, context->render_pass,
        &context->extent, context->framebuffers);

    if (error != SUCCESS) return error;

    error = vulkan_create_command_pool(context->device, context->graphic_index, &context->command_pool);

    if (error != SUCCESS) return error;

    error = vulkan_allocate_command_buffers(
        context->device, context->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 
        (VkCommandBuffer *) &context->command_buffer, 2);

    if (error != SUCCESS) return error;

    return SUCCESS;
}

enum AtomicError vulkan_terminate(const struct VulkanContext *context)
{
    if (context->command_pool)
    {
        vkDestroyCommandPool(context->device, context->command_pool, 0x0);
    }

    if (context->framebuffers)
    {
        for(unsigned index = 0; index < context->swapchain_image_count; ++index)
        {
            if(!context->framebuffers[index]) continue;

            vkDestroyFramebuffer(
                context->device, context->framebuffers[index], 0x0);
        }

        free(context->framebuffers);
    }

    if (context->render_pass)
    {
        vkDestroyRenderPass(context->device, context->render_pass, 0x0);
    }

    if (context->image_views)
    {
        for (unsigned index = 0; index < context->swapchain_image_count; ++index)
        {
            if(!context->image_views[index]) continue;

            vkDestroyImageView(context->device, context->image_views[index], 0x0);
        }

        free(context->image_views);
    }

    if (context->images)
    {
        free(context->images);
    }

    if (context->swapchain)
    {
        vkDestroySwapchainKHR(context->device, context->swapchain, 0x0);
    }

    if (context->device)
    {
        vkDestroyDevice(context->device, 0x0);
    }

    if (context->surface) 
    {
        vkDestroySurfaceKHR(context->instance, context->surface, 0x0);
    }

    if (context->instance)
    {
        vkDestroyInstance(context->instance, 0x0);
    }

    return SUCCESS;
}


#pragma weak vulkan_retrieve_instance_extensions
unsigned vulkan_retrieve_instance_extensions(char ***extensions)
{
    return 0;
}

#pragma weak vulkan_retrieve_instance_layers
unsigned vulkan_retrieve_instance_layers(char ***layers)
{
    return 0;
}

#pragma weak vulkan_retrieve_device_extensions
unsigned vulkan_retrieve_device_extensions(char ***extensions) 
{
    return 0;
}

#pragma weak vulkan_retrieve_device_layers
unsigned vulkan_retrieve_device_layers(char ***layers)
{
    return 0;
}

#pragma weak vulkan_retrieve_device_features
void vulkan_retrieve_device_features(VkPhysicalDeviceFeatures **features)
{
    *features = 0x0;
}

#pragma weak vulkan_select_physical_device
VkPhysicalDevice vulkan_select_physical_device(const VkPhysicalDevice *list, unsigned length)
{
    return list[0];
}

#pragma weak vulkan_retrieve_window_surface
enum AtomicError vulkan_retrieve_window_surface(VkSurfaceKHR *surface)
{
    return SUCCESS;
}

#pragma weak vulkan_retrieve_surface_format
VkSurfaceFormatKHR vulkan_retrieve_surface_format(const VkSurfaceFormatKHR *list, unsigned length)
{
    return list[0];
}

#pragma weak vulkan_retrieve_present_mode
VkPresentModeKHR vulkan_retrieve_present_mode(const VkPresentModeKHR *list, unsigned length)
{
    return VK_PRESENT_MODE_FIFO_KHR; // This one is guaranteed to be available
}
