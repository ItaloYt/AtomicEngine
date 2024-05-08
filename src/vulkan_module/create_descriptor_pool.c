#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_descriptor_pool(
    VkDevice device, VkDescriptorPool *descriptor_pool,
    const VkDescriptorPoolSize *pool_sizes, unsigned pool_count,
    unsigned max_sets)
{
    VkDescriptorPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = pool_count,
        .pPoolSizes = pool_sizes,
        .maxSets = max_sets
    };

    global_result = vkCreateDescriptorPool(
            device, &create_info, 0x0, descriptor_pool
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_DESCRIPTOR_POOL_CREATION_ERROR;

    return SUCCESS;
}
