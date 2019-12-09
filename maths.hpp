
#include "types.hpp"

// ========================================================================
// MATH FUNCTIONS
// ========================================================================
namespace math
{
    inline float pi() { return 3.14159265359f; }
    inline float halfpi() { return 0.5f * math::pi(); }
    inline float twopi() { return 2.f * math::pi(); }
}

// ========================================================================
// VEC3 FUNCTIONS
// ========================================================================
vec3_t operator/(const vec3_t& v, float a);

// ========================================================================
// VEC4 FUNCTIONS
// ========================================================================
namespace vec4
{
    vec4_t vec4(vec3_t xyz, float w);
}

// ========================================================================
// MAT4 FUNCTIONS
// ========================================================================
vec4_t operator*(const mat4_t& m, vec4_t v);
mat4_t operator*(const mat4_t& a, const mat4_t& b);

namespace mat4
{
    mat4_t inverse(const mat4_t& m);
    mat4_t translate(vec3_t t);
    mat4_t rotateY(float a);
    mat4_t perspective(float fovy, float aspect, float zNear, float zFar);
    mat4_t viewport(float x, float y, float width, float height);
}
