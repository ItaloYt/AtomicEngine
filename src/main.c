#include "atomic/game.h"
#include "atomic/input_handle.h"
#include "atomic/types.h"
#include <bits/types/struct_timeval.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <vulkan/vulkan.h>

#include "atomic/util.h"
#include "atomic/window_module.h"
#include "atomic/vulkan_module.h"
#include "atomic/camera.h"

struct WindowContext window_context = {
    .width = 1000,
    .height = 700
};

struct VulkanContext vulkan_context;

struct Camera camera = {
    .far = 100,
    .near = 0.3,
    .view = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 3, 1
    },
    .rotation = {
        .w = 1,
        .x = 0,
        .y = 0,
        .z = 0
    }
};

static char **instance_extensions = 0x0;
static unsigned instance_extension_length = 0;

static char **instance_layers = 0x0;
static unsigned instance_layer_length = 0;

static void quit();

int main()
{
    log_file = stdout;

    atexit(quit);

    enum AtomicError error = window_initialize(&window_context);

    if (error != SUCCESS) return error;

    double frame_start = glfwGetTime();

    error = vulkan_initialize(&vulkan_context);

    if (error != SUCCESS) return error;

    camera_update_projection(&camera, &vulkan_context.extent);

    game_start();

    while (!glfwWindowShouldClose(window_context.handle))
    {
        window_context.delta = glfwGetTime() - frame_start;
        frame_start = glfwGetTime();

        glfwPollEvents();

        error = game_update();

        if(
            window_context.resized ||
            ((error == VULKAN_GET_NEXT_SWAPCHAIN_IMAGE_ERROR || error == VULKAN_QUEUE_PRESENT_ERROR) &&
            global_result == VK_ERROR_OUT_OF_DATE_KHR))
        {
            enum AtomicError error = vulkan_recreate_swapchain(
                vulkan_context.physical_device, vulkan_context.device, 
                &vulkan_context.swapchain, vulkan_context.surface, 
                &vulkan_context.image_count, &vulkan_context.format, 
                &vulkan_context.extent, &vulkan_context.present_mode, 
                vulkan_context.graphic_index, vulkan_context.present_index, 
                &vulkan_context.swapchain_image_count, &vulkan_context.images, 
                &vulkan_context.image_views, vulkan_context.render_pass,
                &vulkan_context.framebuffers);
            
            if (error != SUCCESS) return error;

            camera_update_projection(&camera, &vulkan_context.extent);

            game_resize();

            window_context.resized = 0;

            continue;
        }
        
        if (error != SUCCESS) return error;

        glfwSwapBuffers(window_context.handle);
    }

    global_result = vkDeviceWaitIdle(vulkan_context.device);

    if (global_result != VK_SUCCESS)
    	return VULKAN_DEVICE_WAIT_IDLE_ERROR;

    game_stop();

    return 0;
}

static void quit()
{
    (void) vkDeviceWaitIdle(vulkan_context.device);

    game_exit();

    (void) vulkan_terminate(&vulkan_context);

    (void) window_terminate(&window_context);

    if (instance_layers) free(instance_layers);

    if (instance_extensions) free(instance_extensions);

    //close_error_file();
}

void window_cursor_pos_callback(GLFWwindow *window, double x, double y)
{
    game_cursor_pos(x, y);
}

void window_framebuffer_resize(GLFWwindow *window, int width, int height)
{
    window_context.width = width;
    window_context.height = height;

    window_context.resized = 1;
}

void window_keyboard_callback(
    GLFWwindow *window, int key, int scancode, int action, int modifiers)
{
    game_keyboard(key);
}

unsigned vulkan_retrieve_instance_extensions(char ***extensions)
{
    if(instance_extensions)
    {
        *extensions = instance_extensions;
        return instance_extension_length;
    }

    unsigned length = 0;
    const char *const *window_extensions = 
        window_get_instance_extensions(&length);

    char *desired_extensions[] = {
#ifdef DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    instance_extension_length = length + sizeof(desired_extensions) / sizeof(char *);
    instance_extensions = malloc(instance_extension_length * sizeof(char *));

    if (!instance_extensions)
    {
        (void) log_error("Malloc Instance Extensions\n");

        exit(UNDEFINED);
    }

    (void) memcpy(instance_extensions, 
                  window_extensions, 
                  length * sizeof(char *));

    (void) memcpy(instance_extensions + length, 
                  desired_extensions, 
                  sizeof(desired_extensions));

    *extensions = instance_extensions;
    return instance_extension_length;
}

unsigned vulkan_retrieve_instance_layers(char ***layers)
{
    if (instance_layers)
    {
        *layers = instance_layers;
        return instance_layer_length;
    }

    char *desired_layers[] = {
#ifdef DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };

    instance_layer_length = sizeof(desired_layers) / sizeof(char *);
    instance_layers = malloc(instance_layer_length * sizeof(char *));

    if(!instance_layers) {
        (void) log_error("Malloc Instance Layers\n");

        exit(UNDEFINED);
    }

    (void) memcpy(instance_layers, desired_layers, sizeof(desired_layers));

    *layers = instance_layers;
    return instance_layer_length;
}

enum AtomicError vulkan_retrieve_window_surface(VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(vulkan_context.instance, window_context.handle, 
                                0x0, surface) != VK_SUCCESS)
    {
        (void) log_error("Surface Creation");

        return WINDOW_SURFACE_CREATION_ERROR;
    }

    return SUCCESS;
}

VkSurfaceFormatKHR vulkan_retrieve_surface_format(const VkSurfaceFormatKHR *list, unsigned length)
{
    for (unsigned index = 0; index < length; ++index)
    {
        if(list[index].format == VK_FORMAT_B8G8R8A8_SRGB && 
            list[index].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return list[index];
        }
    }

    (void) log_info("Target Surface Format Not Available\n");

    return list[0];
}

VkPresentModeKHR vulkan_retrieve_present_mode(const VkPresentModeKHR *list, unsigned length)
{
    for (unsigned index = 0; index < length; ++index)
    {
        if (list[index] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return list[index];

        }
    }

    (void) log_info("Target Present Mode Not Available\n");

    return VK_PRESENT_MODE_FIFO_KHR; // This one is guaranteed to be available
}
