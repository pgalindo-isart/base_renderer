#pragma once

struct vec2_t
{
    float x;
    float y;
};

union vec3_t
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    float e[3];
    vec2_t xy;
};

union vec4_t
{
    struct { float x, y, z, w; };
    float e[4];
    vec2_t xy;
    vec3_t xyz;
};

// Column major matrix
union mat4_t
{
    float e[16];
    struct { vec4_t c[4]; };
};

struct rect_t
{
    int x;
    int y;
    int width;
    int height;
};