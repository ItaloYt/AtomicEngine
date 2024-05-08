#ifndef ATOMIC_TYPES_HEADER
#define ATOMIC_TYPES_HEADER

#define PI 3.1415926

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    float x, y, z, w;
} vec4;

typedef struct {
    float w, x, y, z;
} quat;

typedef float mat4[16];

static const vec3 VEC3_UP = { .x = 0, .y = 1, .z = 0 };
static const vec3 VEC3_RIGHT = { .x = 1, .y = 0, .z = 0 };
static const vec3 VEC3_FRONT = { .x = 0, .y = 0, .z = 1 };

float vec3_length(const vec3 *vec);
vec3 vec3_normalize(const vec3 *vec);
vec3 vec3_add(const vec3 *vec1, const vec3 *vec2);
vec3 vec3_scale(const vec3 *vec, float scale);
vec3 vec3_rotate(const vec3 *vec, const quat *q);

float vec3_dot(const vec3 *vec1, const vec3 *vec2);

quat quat_create(const vec3 *axis, float angle);
quat quat_mul(const quat *quat1, const quat *quat2);

float clamp(float value, float min, float max);

#endif
