#include "AABB.h"

AABB::AABB(const Vec3fGrid::Ptr &grid) :
        lower_bnd(grid->transform().indexToWorld(grid->evalActiveVoxelBoundingBox().min())),
        upper_bnd(grid->transform().indexToWorld(grid->evalActiveVoxelBoundingBox().max())) {}


bool AABB::intersect(const Ray &ray, float &t0, float &t1) const {
    float txmin, txmax, tymin, tymax, tzmin, tzmax;
    txmin = (lower_bnd.x() - ray.eye().x()) / ray.dir().x();
    txmax = (upper_bnd.x() - ray.eye().x()) / ray.dir().x();
    tymin = (lower_bnd.y() - ray.eye().y()) / ray.dir().y();
    tymax = (upper_bnd.y() - ray.eye().y()) / ray.dir().y();
    tzmin = (lower_bnd.z() - ray.eye().z()) / ray.dir().z();
    tzmax = (upper_bnd.z() - ray.eye().z()) / ray.dir().z();
    t0 = std::max(std::max(std::min(txmin, txmax),
                           std::min(tymin, tymax)),
                  std::min(tzmin, tzmax));
    t1 = std::min(std::min(std::max(txmin, txmax),
                            std::max(tymin, tymax)),
                   std::max(tzmin, tzmax));

    t0 = std::max(t0, ray.t0());
    t1 = std::min(t1, ray.t1());

    if (t1 < 0) return false;
    return t1 >= t0;

}
