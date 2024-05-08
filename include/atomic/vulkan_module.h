#ifndef VULKAN_CONTEXT_HEADER
#define VULKAN_CONTEXT_HEADER

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "atomic/util.h"

typedef enum AtomicError (*VulkanDrawCallback)(unsigned image_index);

struct VulkanContext {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkRenderPass render_pass;
    VkImage *images;
    VkImageView *image_views;
    VkFramebuffer *framebuffers;
    VkCommandPool command_pool;
    struct {
        VkCommandBuffer draw;
        VkCommandBuffer copy;
    } command_buffer;
    VkQueue graphic_queue;
    VkQueue present_queue;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;
    unsigned image_count;
    unsigned swapchain_image_count;
    unsigned graphic_index;
    unsigned present_index;
};

extern VkResult global_result;

enum AtomicError vulkan_initialize(struct VulkanContext *context);
enum AtomicError vulkan_terminate(const struct VulkanContext *context);
enum AtomicError vulkan_create_instance(VkInstance *instance);
enum AtomicError vulkan_load_physical_devices(
        VkInstance instance, VkPhysicalDevice **list, 
        unsigned *list_length);

enum AtomicError vulkan_load_queue_families(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        unsigned *graphic_index, unsigned *present_index);

enum AtomicError vulkan_create_device(
        VkPhysicalDevice physical_device, VkDevice *device,
        unsigned family_index[], VkQueue *queue[], unsigned family_index_length);

enum AtomicError vulkan_get_surface_information(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        VkExtent2D *extent, VkSurfaceFormatKHR *format, VkPresentModeKHR *present_mode,
        unsigned *image_count);

enum AtomicError vulkan_create_swapchain(
        VkDevice device, VkSwapchainKHR *swapchain, VkSurfaceKHR surface, 
        unsigned image_count, const VkSurfaceFormatKHR *format, const VkExtent2D *extent,
        VkPresentModeKHR present_mode, unsigned graphic_queue_index, unsigned present_queue_index);

enum AtomicError vulkan_load_swapchain_images(
        VkDevice device, VkSwapchainKHR swapchain, unsigned *image_count, 
        VkImage **images, VkImageView **image_views, VkFormat format);

enum AtomicError vulkan_create_shader_module(
        VkDevice device, VkShaderModule *module,
        const unsigned *data, const unsigned data_length);

enum AtomicError vulkan_create_render_pass(VkDevice device, VkFormat format, VkRenderPass *render_pass);

enum AtomicError vulkan_create_descriptor_layout(
        VkDevice device, VkDescriptorSetLayout *descriptor,
        const VkDescriptorSetLayoutBinding *bindings, unsigned binding_count);

enum AtomicError vulkan_create_pipeline_layout(
        VkDevice device, VkPipelineLayout *layout,
        const VkDescriptorSetLayout *descriptors, unsigned descriptor_count);

enum AtomicError vulkan_create_graphic_pipeline(
        VkDevice device, VkRenderPass render_pass, 
        const VkViewport *viewport, const VkRect2D *scissor, 
        VkShaderModule vertex_shader, VkShaderModule fragment_shader, 
        const VkPipelineVertexInputStateCreateInfo *vertex_input_create_info,
        VkPipelineLayout pipeline_layout, VkPipeline *pipeline);

enum AtomicError vulkan_create_descriptor_pool(
        VkDevice device, VkDescriptorPool *descriptor_pool,
        const VkDescriptorPoolSize *pool_sizes, unsigned pool_count,
        unsigned max_sets);

enum AtomicError vulkan_allocate_descriptor_sets(
        VkDevice device, VkDescriptorPool descriptor_pool,
        const VkDescriptorSetLayout *descriptor_layouts, 
        VkDescriptorSet *descriptor_sets, unsigned set_count);

enum AtomicError vulkan_create_framebuffers(
        VkDevice device, const VkImageView *image_views, unsigned image_count, 
        VkRenderPass render_pass, const VkExtent2D *extent, VkFramebuffer *framebuffers);

enum AtomicError vulkan_create_command_pool(VkDevice device, unsigned family_index, VkCommandPool *pool);

enum AtomicError vulkan_allocate_command_buffers(
        VkDevice device, VkCommandPool command_pool, VkCommandBufferLevel level, 
        VkCommandBuffer *command_buffers, unsigned count);

enum AtomicError vulkan_recreate_swapchain(
        VkPhysicalDevice physical_device, VkDevice device, 
        VkSwapchainKHR *swapchain, VkSurfaceKHR surface, 
        unsigned *image_count, VkSurfaceFormatKHR *format, VkExtent2D *extent,
        VkPresentModeKHR *present_mode, unsigned graphic_queue_index, unsigned present_queue_index,
        unsigned *swapchain_image_count, VkImage **images, VkImageView **image_views, 
        VkRenderPass render_pass, VkFramebuffer **framebuffers);

enum AtomicError vulkan_create_buffer(
        VkPhysicalDevice physical_device, VkDevice device,
        VkBuffer *buffer, VkBufferUsageFlags buffer_usage, 
        VkDeviceMemory *memory, unsigned data_size, VkDeviceSize *required_size,
        VkMemoryPropertyFlags memory_preference, VkMemoryPropertyFlags memory_exclude);

enum AtomicError vulkan_create_staged_buffers(
        VkPhysicalDevice physical_device, VkDevice device,
        VkCommandBuffer command_buffer, VkQueue copy_queue, VkBuffer **buffers, 
        const VkBufferUsageFlags *buffer_usages, VkDeviceMemory **memories, 
        const void **datas, const unsigned *data_sizes, VkDeviceSize **required_sizes,
        const VkMemoryPropertyFlags *memory_preferences,
        const VkMemoryPropertyFlags *memory_excludes, unsigned buffer_count);
    

/**
 * @param extensions extension list pointer
 * @return extension count
 */
unsigned vulkan_retrieve_instance_extensions(char ***extensions);

/**
 * @param layers layer list pointer
 * @return layer count
 */
unsigned vulkan_retrieve_instance_layers(char ***layers);

/**
 * @param extensions extension list pointer
 * @return extension count
 */
unsigned vulkan_retrieve_device_extensions(char ***extensions);

/**
 * @param layers layer list pointer
 * @return layer count
 */
unsigned vulkan_retrieve_device_layers(char ***layers);

/**
 * @param features feature list pointer
 */
void vulkan_retrieve_device_features(VkPhysicalDeviceFeatures **features);

/**
 * @param list physical device list
 * @param length physical device list length
 * @return selected physical device
 */
VkPhysicalDevice vulkan_select_physical_device(const VkPhysicalDevice *list, unsigned length);

/**
 * @param surface surface pointer
 * @return error or success code
 */
enum AtomicError vulkan_retrieve_window_surface(VkSurfaceKHR *surface);

/**
 * @param list surface formats list
 * @param length surface format list length
 * @return selected surface format
 */
VkSurfaceFormatKHR vulkan_retrieve_surface_format(const VkSurfaceFormatKHR *list, unsigned length);

/**
 * @param list present modes list
 * @param length present mode list length
 * @return selected present mode
 */
VkPresentModeKHR vulkan_retrieve_present_mode(const VkPresentModeKHR *list, unsigned length);

#endif
