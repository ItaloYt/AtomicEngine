#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_allocate_descriptor_sets(
    VkDevice device, VkDescriptorPool descriptor_pool, 
    const VkDescriptorSetLayout *descriptor_layouts,
    VkDescriptorSet *descriptor_sets,
    unsigned set_count)
{
    VkDescriptorSetAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptor_pool,
        .descriptorSetCount = set_count,
        .pSetLayouts = descriptor_layouts
    };

    global_result = vkAllocateDescriptorSets(
            device, &allocate_info, descriptor_sets);

    if (global_result != VK_SUCCESS)
        return VULKAN_DESCRIPTOR_SETS_ALLOCATION_ERROR;

    return SUCCESS;
}
