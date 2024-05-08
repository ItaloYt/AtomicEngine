#include "atomic/game.h"
#include "atomic/atom.h"
#include "atomic/input_handle.h"
#include "atomic/types.h"
#include "atomic/util.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define RADIAN 0.017453293

struct AtomGroup atom_group = {
    .length = 1
};

static vec3 velocity;
static const float sensibility = 0.1;
static const float speed = 2;

void game_start()
{
    enum AtomicError error = atom_group_create(&vulkan_context, &atom_group);

    if (error != SUCCESS) exit(error);

    (void) memcpy(atom_group.data->projection, camera.projection, sizeof(camera.projection));
    (void) memcpy(atom_group.data->view, camera.view, sizeof(camera.view));

    atom_group_update(&vulkan_context, &atom_group);
}

enum AtomicError game_update()
{
    *(vec3 *) &camera.position = vec3_add((vec3 *) &camera.position, &velocity);

    (void) memcpy(atom_group.data->view, camera.view, sizeof(camera.view));

    enum AtomicError error = atom_group_draw(&vulkan_context, &atom_group);

    if (error != SUCCESS) return error;

    // (void) log_info("FPS: %u\n", (unsigned) (1 / window_context.delta));

    return SUCCESS;
}

void game_keyboard(int key)
{
    // int horizontal = input(A) - input(D);
    int vertical = input(S) - input(W);

    velocity = vec3_scale(&VEC3_FRONT, vertical);
    velocity = vec3_normalize(&velocity);
    velocity = vec3_rotate(&velocity, &camera.rotation);
    velocity = vec3_scale(&velocity, speed * window_context.delta);

    (void) log_info("Rotation: (%f, %f, %f, %f)\n", camera.rotation.w, camera.rotation.x, camera.rotation.y, camera.rotation.z);

    (void) log_info("Velocity: (%f, %f, %f)\n", velocity.x, velocity.y, velocity.z);
}

void game_cursor_pos(double x, double y)
{
    static double last_x = 1000 / 2.0;
    // static double last_y = 700 / 2.0;

    quat zrot = quat_create(&VEC3_FRONT, 0);
    quat yrot = quat_create(&VEC3_UP, (last_x - x) * sensibility);
    quat xrot = quat_create(&VEC3_RIGHT, 0/*(y - last_y) * sensibility*/);

    quat rotator = quat_mul(&xrot, &yrot);
    rotator = quat_mul(&rotator, &zrot);

    camera.rotation = quat_mul(&rotator, &camera.rotation);

    *(vec3 *) &camera.view_forward = vec3_rotate(&VEC3_FRONT, &camera.rotation);
    *(vec3 *) &camera.view_up = vec3_rotate(&VEC3_UP, &camera.rotation);
    *(vec3 *) &camera.view_right = vec3_rotate(&VEC3_RIGHT, &camera.rotation);

    (void) log_info("Rotation: (%f, %f, %f, %f)\n", camera.rotation.w, camera.rotation.x, camera.rotation.y, camera.rotation.z);
    (void) log_info("Forward: (%f, %f, %f)\n", camera.view_forward.x, camera.view_forward.y, camera.view_forward.z);
    (void) log_info("Up: (%f, %f, %f)\n", camera.view_up.x, camera.view_up.y, camera.view_up.z);
    (void) log_info("Right: (%f, %f, %f)\n", camera.view_right.x, camera.view_right.y, camera.view_right.z);

    last_x = x;
    // last_y = y;
}

void game_resize()
{
    (void) memcpy(
        atom_group.data->projection, 
        camera.projection, sizeof(mat4));
}

void game_stop()
{

}


void game_exit()
{
    atom_group_destroy(&vulkan_context, &atom_group);
}

