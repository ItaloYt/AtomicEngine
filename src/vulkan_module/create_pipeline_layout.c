#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_pipeline_layout(
    VkDevice device, VkPipelineLayout *layout, const VkDescriptorSetLayout *descriptors, 
    unsigned descriptor_count)
{
    VkPipelineLayoutCreateInfo layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = descriptor_count,
        .pSetLayouts = descriptors
    };

    global_result = vkCreatePipelineLayout(
            device, &layout_create_info, 0x0, layout
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_PIPELINE_LAYOUT_CREATION_ERROR;

    return SUCCESS;
}
