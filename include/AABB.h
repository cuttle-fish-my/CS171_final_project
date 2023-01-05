#ifndef CS171_FINAL_PROJECT_AABB_H
#define CS171_FINAL_PROJECT_AABB_H

#include "common.h"

struct AABB {
    Vec3f lower_bnd;
    Vec3f upper_bnd;

    explicit AABB(const floatGrid::Ptr &grid);

    bool intersect(const Ray &ray, float &t0, float &t1) const;

    bool isOverlap(const AABB &other) const;
};

struct vdbGrid {
    floatGrid::Ptr grid;
    AABB aabb;
    float dx;
    std::mt19937 gen{std::random_device()()};
    std::normal_distribution<float> opacity_sampler{0, 0.01};
    std::normal_distribution<float> emission_sampler{0, 0.01};


    explicit vdbGrid(const floatGrid::Ptr &grid);
};


#endif //CS171_FINAL_PROJECT_AABB_H
