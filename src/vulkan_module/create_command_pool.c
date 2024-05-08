#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_command_pool(VkDevice device, unsigned int family_index, VkCommandPool *pool)
{
    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = family_index
    };

    global_result = vkCreateCommandPool(device, &create_info, 0x0, pool);

    if (global_result != VK_SUCCESS)
    	return VULKAN_COMMAND_POOL_CREATION_ERROR;

    return SUCCESS;
}
