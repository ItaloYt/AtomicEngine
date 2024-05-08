#include "atomic/types.h"
#include "atomic/util.h"
#include <math.h>

quat quat_create(const vec3 *axis, float angle)
{
    angle *= PI / 180;
    
    vec3 vec = vec3_normalize(axis);
    vec = vec3_scale(&vec, sinf(angle / 2));

    return (quat) {
        .w = cosf(angle / 2),
        .x = vec.x,
        .y = vec.y,
        .z = vec.z
    };
}

quat quat_mul(const quat *quat1, const quat *quat2)
{
    return (quat) {
        .w = quat1->w * quat2->w - quat1->x * quat2->x - quat1->y * quat2->y - quat1->z * quat2->z,
        .x = quat1->w * quat2->x + quat1->x * quat2->w + quat1->y * quat2->z - quat1->z * quat2->y,
        .y = quat1->w * quat2->y + quat1->y * quat2->w + quat1->z * quat2->x - quat1->x * quat2->z,
        .z = quat1->w * quat2->z + quat1->z * quat2->w + quat1->x * quat2->y - quat1->y * quat2->x
    };
}

vec3 vec3_rotate(const vec3 *vec, const quat *q)
{
    quat vector = (quat) { .w = 0, .x = vec->x, .y = vec->y, .z = vec->z };
    quat conjulgate = (quat) { .w = q->w, .x = -q->x, .y = -q->y, .z = -q->z };

    quat result = quat_mul(q, &vector);
    result = quat_mul(&result, &conjulgate);

    return (vec3) {
        .x = result.x,
        .y = result.y,
        .z = result.z
    };
}

float vec3_length(const vec3 *vec)
{
    if (!vec->x && !vec->y && !vec->z) return 0;

    return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

vec3 vec3_normalize(const vec3 *vec)
{
    if (!vec->x && !vec->y && !vec->z) return (vec3) {0};

    float length = vec3_length(vec);

    return (vec3) {
        .x = vec->x / length,
        .y = vec->y / length,
        .z = vec->z / length
    };
}

vec3 vec3_add(const vec3 *vec1, const vec3 *vec2)
{
    return (vec3) {
        .x = vec1->x + vec2->x,
        .y = vec1->y + vec2->y,
        .z = vec1->z + vec2->z
    };
}

vec3 vec3_scale(const vec3 *vec, float scale)
{
    return (vec3) {
        .x = vec->x * scale,
        .y = vec->y * scale,
        .z = vec->z * scale
    };
}

float vec3_dot(const vec3 *vec1, const vec3 *vec2)
{
    return vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
}

float clamp(float value, float min, float max)
{
    return (value < min ? min : (value > max ? max : value));
}
