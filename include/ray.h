#ifndef CS171_FINAL_PROJECT_RAY_H
#define CS171_FINAL_PROJECT_RAY_H
/// Deprecated, use openvdb::math::Ray instead.
/// @Note There are many helper functions of openvdb::math::Ray like:
/// @parameter Ray::operator()
/// @parameter Ray::intersects



/*#include "common.h"
struct Ray : openvdb::math::Ray<float> {
    /// origin point of ray
    Vec3f origin;
    /// normalized direction of the ray
    Vec3f direction;
    /// min and max distance of the ray
    float t_min;
    float t_max;

    explicit Ray(Vec3f o, Vec3f dir, float t_min = RAY_DEFAULT_MIN, float t_max = RAY_DEFAULT_MAX)
            : origin(o), direction(dir), t_min(t_min), t_max(t_max) {}
};*/

#endif //CS171_FINAL_PROJECT_RAY_H
