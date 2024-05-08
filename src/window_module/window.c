#include "atomic/window_module.h"
#include "atomic/util.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

enum AtomicError window_initialize(struct WindowContext *context)
{
    if (!context)
        return INSUFFICIENT_PARAMETERS;

    if (!glfwInit())
        return WINDOW_GLFW_INIT_ERROR;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    context->handle = glfwCreateWindow(context->width, context->height, "Atomic", 0x0, 0x0);

    if (!context->handle)
        return WINDOW_CREATION_ERROR;

    glfwSetCursorPos(context->handle, context->width / 2.0, context->height / 2.0);

    glfwSetFramebufferSizeCallback(context->handle, window_framebuffer_resize);
    glfwSetKeyCallback(context->handle, window_keyboard_callback);
    glfwSetCursorPosCallback(context->handle, window_cursor_pos_callback);

    glfwSetInputMode(context->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(context->handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwMakeContextCurrent(context->handle);

    return SUCCESS;
}

void window_terminate(const struct WindowContext *context)
{
    if(context->handle) glfwDestroyWindow(context->handle);

    glfwTerminate();
};

#pragma weak window_framebuffer_resize
void window_framebuffer_resize(GLFWwindow *window, int width, int height) {}

#pragma weak window_keyboard_callback
void window_keyboard_callback(
    GLFWwindow *window, int key, int scancode, int action, int modifiers) {}

#pragma weak window_cursor_pos_callback
void window_cursor_pos_callback(GLFWwindow *window, double x, double y) {}
