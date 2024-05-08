#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_allocate_command_buffers(
    VkDevice device, VkCommandPool command_pool, 
    VkCommandBufferLevel level, VkCommandBuffer *command_buffers,
    unsigned count)
{
    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = level,
        .commandBufferCount = count
    };

    global_result = vkAllocateCommandBuffers(device, &allocate_info, command_buffers);

    if (global_result != VK_SUCCESS)
        return VULKAN_COMMAND_BUFFERS_ALLOCATION_ERROR;

    return SUCCESS;
}
