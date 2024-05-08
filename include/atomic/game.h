#ifndef ATOMIC_GAME_HEADER
#define ATOMIC_GAME_HEADER

#include "atomic/atom.h"
#include "atomic/camera.h"
#include "atomic/util.h"
#include "atomic/vulkan_module.h"
#include "atomic/window_module.h"

extern struct VulkanContext vulkan_context;
extern struct WindowContext window_context;
extern struct Camera camera;
extern struct AtomGroup atom_group;

void game_start();
enum AtomicError game_update();
void game_exit();
void game_stop();

void game_resize();
void game_keyboard(int key);
void game_cursor_pos(double x, double y);

#endif
