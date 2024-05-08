#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_descriptor_layout(
    VkDevice device, VkDescriptorSetLayout *descriptor, 
    const VkDescriptorSetLayoutBinding *bindings, unsigned int binding_count)
{
    VkDescriptorSetLayoutCreateInfo descriptor_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = binding_count,
        .pBindings = bindings
    };

    global_result = vkCreateDescriptorSetLayout(
        device, &descriptor_create_info, 0x0, descriptor);

    if (global_result != VK_SUCCESS)
    	return VULKAN_DESCRIPTOR_LAYOUT_CREATION_ERROR;

    return SUCCESS;
}
