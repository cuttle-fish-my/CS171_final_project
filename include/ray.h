#ifndef CS171_FINAL_PROJECT_RAY_H
#define CS171_FINAL_PROJECT_RAY_H
#include "common.h"

struct Ray {
    /// origin point of ray
    Vec3f origin;
    /// normalized direction of the ray
    Vec3f direction;
    /// min and max distance of the ray
    float t_min;
    float t_max;

    explicit Ray(Vec3f o, Vec3f dir, float t_min = RAY_DEFAULT_MIN, float t_max = RAY_DEFAULT_MAX)
            : origin(std::move(o)), direction(std::move(dir)), t_min(t_min), t_max(t_max) {}

    [[nodiscard]] Vec3f operator()(float t) const {
        return origin + t * direction;
    }
};

#endif //CS171_FINAL_PROJECT_RAY_H
