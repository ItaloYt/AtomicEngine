#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_shader_module(VkDevice device, VkShaderModule *module,
                                             const unsigned int *data, const unsigned int data_length)
{
    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = data,
        .codeSize = data_length
    };

    global_result = vkCreateShaderModule(device, &create_info, 0x0, module);

    if (global_result != VK_SUCCESS)
    	return VULKAN_SHADER_MODULE_CREATION_ERROR;

    return SUCCESS;
}
