#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

static const char *const required_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

enum AtomicError vulkan_create_device(VkPhysicalDevice physical_device, VkDevice *device, 
                                      unsigned family_index[], VkQueue *queue[], unsigned family_index_length)
{
    VkDeviceQueueCreateInfo queue_info[family_index_length];

    const float queue_priority = 1;

    for(unsigned index = 0; index < family_index_length; ++index)
    {
        queue_info[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[index].queueCount = 1;
        queue_info[index].queueFamilyIndex = family_index[index];
        queue_info[index].pQueuePriorities = &queue_priority;
        queue_info[index].flags = 0x0;
        queue_info[index].pNext = 0x0;
    }

    char **device_extensions = 0x0;
    unsigned device_extension_length = 
        vulkan_retrieve_device_extensions(&device_extensions);

    unsigned extension_length = 
        device_extension_length + 
        sizeof(required_extensions) / sizeof(char *);

    char **extensions = alloca(extension_length * sizeof(char *));

    // it is impossible to check for errors using alloca
    (void) memcpy(
        extensions, 
        device_extensions, 
        device_extension_length * sizeof(char *));

    (void) memcpy(
        extensions + device_extension_length,
        required_extensions,
        sizeof(required_extensions));

    char **device_layers = 0x0;

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queue_info,
        .queueCreateInfoCount = family_index_length,
        .enabledExtensionCount = extension_length,
        .ppEnabledExtensionNames = (const char *const *) extensions,
        .enabledLayerCount = vulkan_retrieve_device_layers(&device_layers),
        .ppEnabledLayerNames = (const char *const *) device_layers,
        .pEnabledFeatures = 0x0
    };

    vulkan_retrieve_device_features((VkPhysicalDeviceFeatures **) &create_info.pEnabledFeatures);

    global_result = vkCreateDevice(physical_device, &create_info, 0x0, device);

    if (global_result != VK_SUCCESS)
    	return VULKAN_DEVICE_CREATION_ERROR;

    for (unsigned index = 0; index < family_index_length; ++index) 
    {
        vkGetDeviceQueue(*device, family_index[index], 0, queue[index]);
    }

    return SUCCESS;
}
