#include "atomic/vulkan_module.h"
#include "atomic/util.h"
#include <stdio.h>
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_instance(VkInstance *instance) 
{
    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_3,
        .pEngineName = 0x0,
        .engineVersion = 0,
        .pApplicationName = "Atomic Engine",
        .applicationVersion = 1
    };

    char **instance_extensions = 0x0;
    char **instance_layers = 0x0;

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = vulkan_retrieve_instance_layers(&instance_layers),
        .ppEnabledLayerNames = (const char *const *) instance_layers,
        .enabledExtensionCount = vulkan_retrieve_instance_extensions(&instance_extensions),
        .ppEnabledExtensionNames = (const char *const *) instance_extensions
    };

    global_result = vkCreateInstance(&instance_create_info, 0x0, instance);

    if (global_result != VK_SUCCESS)
    	return VULKAN_INSTANCE_CREATION_ERROR;

    return SUCCESS;
}
