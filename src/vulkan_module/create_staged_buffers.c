#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_staged_buffers(
    VkPhysicalDevice physical_device, VkDevice device, 
    VkCommandBuffer command_buffer, VkQueue copy_queue, VkBuffer **buffers, 
    const VkBufferUsageFlags *buffer_usages, VkDeviceMemory **memories, 
    const void **datas, const unsigned int *data_sizes, 
    VkDeviceSize **required_sizes, const VkMemoryPropertyFlags *memory_preferences, 
    const VkMemoryPropertyFlags *memory_excludes, unsigned buffer_count)
{

    enum AtomicError error = SUCCESS;

    void *mapped = 0x0;
    
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    global_result = vkBeginCommandBuffer(command_buffer, &begin_info);
    
    if (global_result != VK_SUCCESS)
        return VULKAN_COMMAND_BUFFER_BEGIN_ERROR;

    VkBuffer stage_buffers[buffer_count];
    VkDeviceMemory stage_memories[buffer_count];
    VkDeviceSize stage_sizes[buffer_count];

    for (unsigned index = 0; index < buffer_count; ++index) 
    {
        error = vulkan_create_buffer(
            physical_device, device, buffers[index], buffer_usages[index] |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT, memories[index], 
            data_sizes[index], required_sizes[index],
            memory_preferences[index], memory_excludes[index]);

        if (error != SUCCESS) return error;

        error = vulkan_create_buffer(
            physical_device, device, &stage_buffers[index],
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            &stage_memories[index], data_sizes[index], &stage_sizes[index],
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);

        if (error != SUCCESS) return error;

        global_result = vkMapMemory(
            device, stage_memories[index],
            0, stage_sizes[index], 0, &mapped);
        
        if (global_result != VK_SUCCESS)
            return VULKAN_MAP_MEMORY_ERROR;

        (void) memcpy(mapped, datas[index], data_sizes[index]);

        vkUnmapMemory(device, stage_memories[index]);

        vkCmdCopyBuffer(
            command_buffer, stage_buffers[index], *(buffers[index]), 1,
            (VkBufferCopy[]) {
                (VkBufferCopy) {
                    .size = data_sizes[index],
                    .dstOffset = 0,
                    .srcOffset = 0
                }
            });
    }

    global_result = vkEndCommandBuffer(command_buffer);
    
    if (global_result != VK_SUCCESS)
        return VULKAN_COMMAND_BUFFER_END_ERROR;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = (VkCommandBuffer[]) {
            command_buffer
        }
    };

    global_result = vkQueueSubmit(
        copy_queue, 1,
        (VkSubmitInfo[]) {
                submit_info
        }, 0x0);

    if (global_result != VK_SUCCESS)
        return VULKAN_QUEUE_SUBMITION_ERROR;

    global_result = vkQueueWaitIdle(copy_queue);
    
    if (global_result != VK_SUCCESS)
        return VULKAN_QUEUE_WAIT_ERROR;

    for (unsigned index = 0; index < buffer_count; ++index)
    {
        vkFreeMemory(device, stage_memories[index], 0x0);
        vkDestroyBuffer(device, stage_buffers[index], 0x0);
    }

    return SUCCESS;
}
