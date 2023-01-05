#ifndef CS171_FINAL_PROJECT_AABB_H
#define CS171_FINAL_PROJECT_AABB_H

#include "common.h"

struct AABB {
    Vec3f lower_bnd;
    Vec3f upper_bnd;

    AABB() = default;

    AABB(Vec3f lower, Vec3f upper);

    explicit AABB(const floatGrid::Ptr &grid);

    bool intersect(const Ray &ray, float &t0, float &t1) const;

    [[nodiscard]] bool isOverlap(const AABB &other) const;

    [[nodiscard]] bool inAABB(Vec3f point) const;
};

struct vdbGrid {
    floatGrid::Ptr grid;
    AABB aabb;
    float dx;


    explicit vdbGrid(const floatGrid::Ptr &grid);
};

struct KDTree {
    bool leaf;
    AABB aabb;
    int support_layer;
    std::vector<KDTree> children;
};


#endif //CS171_FINAL_PROJECT_AABB_H
