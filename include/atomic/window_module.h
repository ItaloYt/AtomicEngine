#ifndef WINDOW_CONTEXT_HEADER
#define WINDOW_CONTEXT_HEADER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "atomic/util.h"

#define window_get_instance_extensions(length) glfwGetRequiredInstanceExtensions(length)
#define window_is_closed(handle) glfwWindowShouldClose(handle)

struct WindowContext {
    GLFWwindow *handle;
    double delta;
    unsigned width;
    unsigned height;

    char resized;
};

enum AtomicError window_initialize(struct WindowContext *context);
void window_terminate(const struct WindowContext *context);

void window_framebuffer_resize(GLFWwindow *window, int width, int height);
void window_keyboard_callback(
        GLFWwindow *window, int key, int scancode, int action, int modifiers);

void window_cursor_pos_callback(GLFWwindow *window, double x, double y);

#endif
