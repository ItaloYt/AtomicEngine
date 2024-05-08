#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_buffer(VkPhysicalDevice physical_device, VkDevice device, 
                                      VkBuffer *buffer, VkBufferUsageFlags buffer_usage,
                                      VkDeviceMemory *memory, unsigned int data_size, 
                                      VkDeviceSize *required_size,
                                      VkMemoryPropertyFlags memory_preference, 
                                      VkMemoryPropertyFlags memory_exclude)
{
    VkBufferCreateInfo vertex_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = data_size,
        .usage = buffer_usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    global_result = vkCreateBuffer(
            device, &vertex_buffer_create_info, 0x0, buffer);

    if (global_result != VK_SUCCESS)
        return VULKAN_BUFFER_CREATION_ERROR;

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, *buffer, &requirements);

    *required_size = requirements.size;

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &properties);

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = 0 // It will be used as a flag
    };

    for (unsigned index = 0; index < properties.memoryTypeCount; ++index)
    {
        if (
            (requirements.memoryTypeBits & (1 << index)) && 
            (properties.memoryTypes[index].propertyFlags & memory_preference) &&
            !(properties.memoryTypes[index].propertyFlags & memory_exclude))
        {
            allocate_info.allocationSize = *required_size;
            allocate_info.memoryTypeIndex = index;
        }
    }

    if (!allocate_info.allocationSize)
        return VULKAN_ALLOCATE_MEMORY_ERROR;

    global_result = vkAllocateMemory(device, &allocate_info, 0x0, memory);
        
    if (global_result != VK_SUCCESS) 
        return VULKAN_ALLOCATE_MEMORY_ERROR;

    global_result = vkBindBufferMemory(device, *buffer, *memory, 0);

    if (global_result != VK_SUCCESS)
    	return VULKAN_BIND_BUFFER_MEMORY_ERROR;

    return SUCCESS;
}
