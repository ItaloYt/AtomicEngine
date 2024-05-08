#include "atomic/atom.h"
#include "atomic/spirv.h"
#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

enum AtomicError atom_group_create(
    const struct VulkanContext *context, struct AtomGroup *group)
{
    group->atoms = calloc(group->length, sizeof(struct Atom));

    if (!group->atoms)
    {
        (void) log_error("Group Atoms Allocation Error\n");

        return MALLOC_ERROR;
    }

    enum AtomicError error = vulkan_create_shader_module(
        context->device, &group->vertex_shader,
        base_vert_spv, base_vert_spv_length);

    if (error != SUCCESS) return error;

    error = vulkan_create_shader_module(
        context->device, &group->fragment_shader, 
        base_frag_spv, base_frag_spv_length);

    if (error != SUCCESS) return error;

    error = vulkan_create_descriptor_layout(
        context->device, &group->descriptor.layout,
        (VkDescriptorSetLayoutBinding[]) {
            (VkDescriptorSetLayoutBinding) {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
            }
        }, 1);

    if (error != SUCCESS) return error;

    error = vulkan_create_pipeline_layout(
        context->device, &group->pipeline.layout,
        (VkDescriptorSetLayout[]) {
            group->descriptor.layout
        }, 1);

    if (error != SUCCESS) return error;

    error = vulkan_create_graphic_pipeline(
        context->device, context->render_pass, 
        &(VkViewport) {
            .width = context->extent.width,
            .height = context->extent.height,
            .maxDepth = 1
        },
        &(VkRect2D) {
            .extent = context->extent
        },
        group->vertex_shader, group->fragment_shader,
        &(VkPipelineVertexInputStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = (VkVertexInputBindingDescription[]) {
                (VkVertexInputBindingDescription) {
                    .binding = 0,
                    .stride = atom_vertex_stride,
                    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
                }
            },
            .vertexAttributeDescriptionCount = 1,
            .pVertexAttributeDescriptions = (VkVertexInputAttributeDescription[]) {
                (VkVertexInputAttributeDescription) {
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = 0,
                    .location = 0
                }
            }
        },

        group->pipeline.layout, &group->pipeline.handle);

    if (error != SUCCESS) return error;
    
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    
    global_result = vkCreateSemaphore(
            context->device, &semaphore_create_info, 0x0, &group->semaphore1
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_SEMAPHORE_CREATION_ERROR;

    global_result = vkCreateSemaphore(
            context->device, &semaphore_create_info, 0x0, &group->semaphore2
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_SEMAPHORE_CREATION_ERROR;

    global_result = vkCreateFence(
            context->device, &fence_create_info, 0x0, &group->fence1
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_FENCE_CREATION_ERROR;

    error = vulkan_create_staged_buffers(
        context->physical_device, context->device,
        context->command_buffer.copy, context->graphic_queue,
        (VkBuffer *[]) {
            &group->vertex_buffer.handle,
            &group->index_buffer.handle
        },
        (VkBufferUsageFlags[]) {
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        },
        (VkDeviceMemory *[]) {
            &group->vertex_buffer.memory,
            &group->index_buffer.memory
        },
        (const void *[]) {
            (void *) atom_vertexs,
            (void *) atom_indexs
        },
        (unsigned[]) {
            sizeof(atom_vertexs),
            sizeof(atom_indexs)
        },
        (VkDeviceSize *[]) {
            &group->vertex_buffer.size,
            &group->index_buffer.size
        },
        (VkMemoryPropertyFlags[]) {
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        },
        (VkMemoryPropertyFlags[]) {
            VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD,
            VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD
        }, 2);

    if (error != SUCCESS) return error;

    error = vulkan_create_buffer(
        context->physical_device, context->device,
        &group->uniform_buffer.handle, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        &group->uniform_buffer.memory, sizeof(struct AtomGroupData),
        &group->uniform_buffer.size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);

    if (error != SUCCESS) return error;

    global_result = vkMapMemory(
            context->device, group->uniform_buffer.memory, 0,
            sizeof(struct AtomGroupData), 0, (void **) &group->uniform_buffer.mapped
        );

    if (global_result != VK_SUCCESS)
    	return VULKAN_MAP_MEMORY_ERROR;

    group->data = group->uniform_buffer.mapped;

    // struct Atom *atom;
    //
    // for (unsigned atom_index = 0; atom_index < group->length; ++atom_index)
    // {
    //     atom = group->atoms + atom_index;
    //
    //     error = vulkan_create_buffer(
    //         context->physical_device, context->device,
    //         &atom->uniform.handle, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    //         &atom->uniform.memory, sizeof(struct AtomData),
    //         &atom->uniform.size,
    //         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    //         VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);
    //
    //     if (error != SUCCESS) return error;
    //
    //     check_vulkan(
    //         vkMapMemory(
    //             context->device, atom->uniform.memory, 0,
    //             sizeof(struct AtomData), 0, &atom->uniform.mapped
    //         ),
    //         VULKAN_MAP_MEMORY_ERROR,
    //         "Map Memory Error\n");
    // }

    error = vulkan_create_descriptor_pool(
        context->device, &group->descriptor.pool,
        (VkDescriptorPoolSize[]) {
            (VkDescriptorPoolSize) {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1
            }
        }, 1, group->length + 1);

    if (error != SUCCESS) return error;

    // group->sets = calloc(group->length, sizeof(VkDescriptorSet));
    //
    // if (!group->sets)
    // {
    //     (void) error("Group Sets Allocation Error\n");
    //
    //     return MALLOC_ERROR;
    // }

    // VkDescriptorSetLayout layouts[group->length];
    //
    // for (unsigned index = 0; index < group->length; ++index)
    // {
    //     layouts[index] = group->descriptor.layout;
    // }
    //
    // error = vulkan_allocate_descriptor_sets(
    //     context->device, group->descriptor.pool, 
    //     layouts, group->sets, group->length);

    // if (error != SUCCESS) return error;

    error = vulkan_allocate_descriptor_sets(
        context->device, group->descriptor.pool,
        (VkDescriptorSetLayout[]) {
            group->descriptor.layout
        }, &group->descriptor.set, 1);

    if (error != SUCCESS) return error;

    return SUCCESS;
}

enum AtomicError atom_group_draw(
    struct VulkanContext *context, const struct AtomGroup *group)
{
    // Semaphore1 - is image available
    // Semaphore2 - is render finished
    // Fence1 - is frame end

    global_result = vkWaitForFences(
        context->device, 1, 
        (VkFence[]) {
            group->fence1
        }, 1, UINT64_MAX);

    if (global_result != VK_SUCCESS)
    	return VULKAN_FENCES_WAIT_ERROR;

    unsigned image_index;

    global_result = vkAcquireNextImageKHR(
        context->device, context->swapchain, UINT64_MAX,
        group->semaphore1, 0x0, &image_index);

    if (global_result != VK_SUCCESS && global_result != VK_SUBOPTIMAL_KHR)
        return VULKAN_GET_NEXT_SWAPCHAIN_IMAGE_ERROR;

    global_result = vkResetFences(
        context->device, 1,
        (VkFence[]) {
            group->fence1
        });

    if (global_result != VK_SUCCESS)
    	return VULKAN_FENCES_RESET_ERROR;

    global_result = vkResetCommandBuffer(context->command_buffer.draw, 0);

    if (global_result != VK_SUCCESS)
    	return VULKAN_COMMAND_BUFFER_RESET_ERROR;

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    global_result = vkBeginCommandBuffer(context->command_buffer.draw, &command_buffer_begin_info);

    if (global_result != VK_SUCCESS)
    	return VULKAN_COMMAND_BUFFER_BEGIN_ERROR;

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = context->render_pass,
        .renderArea = {
            .extent = context->extent
        },
        .framebuffer = context->framebuffers[image_index],
        .clearValueCount = 1,
        .pClearValues = (VkClearValue[]) {
            (VkClearValue) {0}
        }
    };

    vkCmdBeginRenderPass(
        context->command_buffer.draw, &render_pass_begin_info, 
        VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(
        context->command_buffer.draw, 0, 1,
        (VkViewport[]) {
            (VkViewport) {
                .width = context->extent.width,
                .height = context->extent.height,
                .maxDepth = 1
            }
        });
    vkCmdSetScissor(
        context->command_buffer.draw, 0, 1,
        (VkRect2D[]) {
            (VkRect2D) {
                .extent = context->extent
            }
        });
    vkCmdBindPipeline(
        context->command_buffer.draw, VK_PIPELINE_BIND_POINT_GRAPHICS, 
        group->pipeline.handle);
    vkCmdBindVertexBuffers(
        context->command_buffer.draw, 0, 1,
        (VkBuffer[]) {
            group->vertex_buffer.handle
        },
        (VkDeviceSize[]) {
            0
        });
    vkCmdBindIndexBuffer(
        context->command_buffer.draw, group->index_buffer.handle,
        0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(
        context->command_buffer.draw, VK_PIPELINE_BIND_POINT_GRAPHICS,
        group->pipeline.layout, 0, 1,
        (VkDescriptorSet[]) {
            group->descriptor.set
        }, 0, 0x0);

    for (unsigned index = 0; index < group->length; ++index)
    {
        // vkCmdBindDescriptorSets(
        //     context->command_buffer.draw, VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     group->pipeline.layout, 0, 1,
        //     (VkDescriptorSet[]) {
        //         group->sets[index]
        //     }, 0, 0x0);
        vkCmdDrawIndexed(
            context->command_buffer.draw, 
            sizeof(atom_indexs) / sizeof(atom_indexs[0]), 
            1, 0, 0, 0);
    }

    vkCmdEndRenderPass(context->command_buffer.draw);

    global_result = vkEndCommandBuffer(context->command_buffer.draw);

    if (global_result != VK_SUCCESS)
    	return VULKAN_COMMAND_BUFFER_END_ERROR;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]) {
            group->semaphore1
        },
        .pWaitDstStageMask = (VkPipelineStageFlags[]) {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        },
        .commandBufferCount = 1,
        .pCommandBuffers = (VkCommandBuffer[]) {
            context->command_buffer.draw
        },
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = (VkSemaphore[]) {
            group->semaphore2
        }
    };

    global_result = vkQueueSubmit(
        context->graphic_queue, 1,
        (VkSubmitInfo[]) {
            submit_info
        },
        group->fence1);

    if (global_result != VK_SUCCESS)
    	return VULKAN_QUEUE_SUBMITION_ERROR;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (VkSemaphore[]) {
            group->semaphore2
        },
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]) {
            context->swapchain
        },
        .pImageIndices = (unsigned[]) {
            image_index
        }
    };

    global_result = vkQueuePresentKHR(context->present_queue, &present_info);

    if (global_result != VK_SUCCESS && global_result != VK_SUBOPTIMAL_KHR)
        return VULKAN_QUEUE_PRESENT_ERROR;

    return SUCCESS;
}

void atom_group_update(
    const struct VulkanContext *context, const struct AtomGroup *group)
{
    VkDescriptorBufferInfo buffer_info = {
        .range = sizeof(struct AtomGroupData),
        .offset = 0,
        .buffer = group->uniform_buffer.handle
    };

    VkWriteDescriptorSet write_descriptor = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = group->descriptor.set,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &buffer_info
    };

    vkUpdateDescriptorSets(
        context->device, 1,
        (VkWriteDescriptorSet[]) {
            write_descriptor
        }, 0, 0x0);
}

void atom_group_destroy(
    const struct VulkanContext *context, const struct AtomGroup *group)
{
    if (group->descriptor.pool)
    {
        vkDestroyDescriptorPool(
            context->device, group->descriptor.pool, 0x0);
    }

    // for (unsigned atom_index = 0; atom_index < group->length; ++atom_index)
    // {
    //     if (group->atoms[atom_index].uniform.memory)
    //     {
    //         vkFreeMemory(
    //             context->device, group->atoms[atom_index].uniform.memory, 0x0);
    //     }
    //
    //     if (group->atoms[atom_index].uniform.handle)
    //     {
    //         vkDestroyBuffer(
    //             context->device, group->atoms[atom_index].uniform.handle, 0x0);
    //     }
    // }

    if (group->uniform_buffer.memory)
    {
        vkFreeMemory(context->device, group->uniform_buffer.memory, 0x0);
    }

    if (group->uniform_buffer.handle)
    {
        vkDestroyBuffer(context->device, group->uniform_buffer.handle, 0x0);
    }

    if (group->index_buffer.memory)
    {
        vkFreeMemory(context->device, group->index_buffer.memory, 0x0);
    }

    if (group->index_buffer.handle)
    {
        vkDestroyBuffer(context->device, group->index_buffer.handle, 0x0);
    }

    if (group->vertex_buffer.memory)
    {
        vkFreeMemory(context->device, group->vertex_buffer.memory, 0x0);
    }

    if (group->vertex_buffer.handle)
    {
        vkDestroyBuffer(context->device, group->vertex_buffer.handle, 0x0);
    }

    if (group->fence1)
    {
        vkDestroyFence(context->device, group->fence1, 0x0);
    }

    if (group->semaphore2)
    {
        vkDestroySemaphore(context->device, group->semaphore2, 0x0);
    }

    if (group->semaphore1)
    {
        vkDestroySemaphore(context->device, group->semaphore1, 0x0);
    }

    if (group->pipeline.handle)
    {
        vkDestroyPipeline(context->device, group->pipeline.handle, 0x0);
    }

    if (group->pipeline.layout)
    {
        vkDestroyPipelineLayout(
            context->device, group->pipeline.layout, 0x0);
    }

    if (group->descriptor.layout)
    {
        vkDestroyDescriptorSetLayout(
            context->device, group->descriptor.layout, 0x0);
    }

    if (group->fragment_shader)
    {
        vkDestroyShaderModule(
            context->device, group->fragment_shader, 0x0);
    }

    if (group->vertex_shader)
    {
        vkDestroyShaderModule(context->device, group->vertex_shader, 0x0);
    }
}
