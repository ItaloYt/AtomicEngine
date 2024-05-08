#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

enum AtomicError vulkan_create_framebuffers(VkDevice device, const VkImageView *image_views,
                                            unsigned int image_count, VkRenderPass render_pass,
                                            const VkExtent2D *extent, VkFramebuffer *framebuffers)
{
    VkFramebufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .width = extent->width,
        .height = extent->height,
        .layers = 1,
        .renderPass = render_pass,
        .attachmentCount = 1,
    };

    for (unsigned index = 0; index < image_count; ++index)
    {
        create_info.pAttachments = (VkImageView[]) { image_views[index] };

        global_result = vkCreateFramebuffer(device, &create_info, 0x0, framebuffers + index);

        if (global_result != VK_SUCCESS)
        	return VULKAN_FRAMEBUFFER_CREATION_ERROR;
    }

    return SUCCESS;
}
