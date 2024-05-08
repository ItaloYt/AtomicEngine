#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_get_surface_information(VkPhysicalDevice physical_device, 
                                                VkSurfaceKHR surface,
                                                VkExtent2D *extent, 
                                                VkSurfaceFormatKHR *format, 
                                                VkPresentModeKHR *present_mode, unsigned *image_count)
{
    VkSurfaceCapabilitiesKHR surface_capabilities;

    global_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device, surface, &surface_capabilities);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SURFACE_CAPABILITIES_ERROR;

    *extent = surface_capabilities.currentExtent;

    if (extent->width == UINT32_MAX)
        return NOT_IMPLEMENTED_BEHAVIOUR;

    *image_count = surface_capabilities.minImageCount + 1;

    if (surface_capabilities.maxImageCount > 0 &&
        *image_count > surface_capabilities.maxImageCount)
    {
        *image_count = surface_capabilities.maxImageCount;
    }

    unsigned surface_format_length;

    global_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, surface, &surface_format_length, 0x0);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SURFACE_FORMATS_ERROR;

    VkSurfaceFormatKHR surface_formats[surface_format_length];

    global_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, surface, &surface_format_length, 
        surface_formats);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SURFACE_FORMATS_ERROR;

    *format = vulkan_retrieve_surface_format(surface_formats, surface_format_length);

    unsigned present_mode_length;

    global_result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface, &present_mode_length, 0x0);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SURFACE_PRESENT_MODES_ERROR;

    VkPresentModeKHR present_modes[present_mode_length];

    global_result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface,  &present_mode_length, present_modes);

    if (global_result != VK_SUCCESS)
        return VULKAN_GET_SURFACE_PRESENT_MODES_ERROR;

    *present_mode = vulkan_retrieve_present_mode(present_modes, present_mode_length);

    return SUCCESS;
}
