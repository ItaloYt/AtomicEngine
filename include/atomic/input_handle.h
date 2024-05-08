#ifndef ATOMIC_INPUT_HANDLE_HEADER
#define ATOMIC_INPUT_HANDLE_HEADER

#define input(letter) glfwGetKey(window_context.handle, GLFW_KEY_##letter)

#endif
