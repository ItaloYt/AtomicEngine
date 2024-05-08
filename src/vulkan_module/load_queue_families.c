#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_load_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                                            unsigned *graphic_index, unsigned *present_index)
{
    unsigned family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, 0x0);

    VkQueueFamilyProperties families[family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, families);

    unsigned char flag = 00; //present - graphic
        
    for (unsigned family_index = 0; family_index < family_count && flag < 3; ++family_index)
    {
        if ((families[family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            && (flag ^ 1))
        {
            *graphic_index = family_index;
            flag |= 1;
        }

        if (flag & 2) continue;

        unsigned is_present_supported;

        global_result = vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, family_index,  surface, 
            &is_present_supported);

        if (global_result != VK_SUCCESS)
            return VULKAN_CHECK_SURFACE_SUPPORT_ERROR;

        if (!is_present_supported) continue;

        *present_index = family_index;
        flag |= 2;
    }

    return SUCCESS;
}
