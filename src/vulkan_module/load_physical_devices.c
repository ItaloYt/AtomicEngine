#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <stdlib.h>

enum AtomicError vulkan_load_physical_devices(VkInstance instance, VkPhysicalDevice **list, unsigned *list_length) 
{
    global_result = vkEnumeratePhysicalDevices(instance, list_length, 0x0);

    if (global_result != VK_SUCCESS)
        return VULKAN_LOADING_PHYSICAL_DEVICES_ERROR;

    *list = malloc(*list_length * sizeof(VkPhysicalDevice));

    if (!*list)
        return MALLOC_ERROR;

    global_result = vkEnumeratePhysicalDevices(instance, list_length, *list);

    if (global_result != VK_SUCCESS)
    {
        free(*list);

        return VULKAN_LOADING_PHYSICAL_DEVICES_ERROR;
    }

    return SUCCESS;
}
