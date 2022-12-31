#ifndef CS171_FINAL_PROJECT_AABB_H
#define CS171_FINAL_PROJECT_AABB_H

#include "common.h"

struct AABB {
    Vec3f lower_bnd;
    Vec3f upper_bnd;
    explicit AABB(const Vec3fGrid::Ptr& grid);

    bool intersect(const Ray &ray, float &t0, float &t1) const;
};

#endif //CS171_FINAL_PROJECT_AABB_H
