#ifndef ATOMIC_ATOM_HEADER
#define ATOMIC_ATOM_HEADER

#include "atomic/types.h"
#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include <vulkan/vulkan_core.h>

static const float atom_vertexs[] = {
    -0.5, -0.5,
    0.5, -0.5,
    0.5, 0.5,
    -0.5, 0.5
};

static const unsigned atom_indexs[] = {
    0, 1, 2,
    2, 3, 0
};

static const unsigned atom_vertex_stride = 2 * sizeof(atom_vertexs[0]);

struct AtomBuffer {
    VkBuffer handle;
    VkDeviceMemory memory;

    void *mapped;

    VkDeviceSize size;
};

struct AtomPipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
};

struct AtomDescriptor {
    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;
    VkDescriptorSet set;
};

struct AtomData {};

struct Atom {
    struct AtomData data;
    struct AtomBuffer uniform;
};

struct AtomGroupData {
    mat4 projection;
    mat4 view;
};

struct AtomGroup {
    struct AtomGroupData *data;

    struct AtomPipeline pipeline;
    struct AtomBuffer vertex_buffer;
    struct AtomBuffer index_buffer;
    struct {
        VkBuffer handle;
        VkDeviceMemory memory;

        struct AtomGroupData *mapped;

        VkDeviceSize size;
    } uniform_buffer;

    struct AtomDescriptor descriptor;

    struct Atom *atoms;
    // VkDescriptorSet *atom_sets;

    VkSemaphore semaphore1;
    VkSemaphore semaphore2;
    VkFence fence1;

    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;

    unsigned length;
};

enum AtomicError atom_group_create(
        const struct VulkanContext *context, struct AtomGroup *group);

enum AtomicError atom_group_draw(
        struct VulkanContext *context, const struct AtomGroup *atom);

void atom_group_update(
        const struct VulkanContext *context, const struct AtomGroup *atom);

void atom_group_destroy(
        const struct VulkanContext *context, const struct AtomGroup *group);

#endif
