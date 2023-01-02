#include "AABB.h"

AABB::AABB(const floatGrid::Ptr &grid) :
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

vdbGrid::vdbGrid(const floatGrid::Ptr &grid) : grid(grid), aabb(grid), dx(float(grid->transform().voxelSize()[0])) {
}

float vdbGrid::interpolation(const Vec3f &pos) const {
    Vec3f local_pos = grid->transform().worldToIndex(pos);
    Vec3f lower = Vec3f(floor(local_pos.x()), floor(local_pos.y()), floor(local_pos.z()));
    Vec3f upper = Vec3f(ceil(local_pos.x()), ceil(local_pos.y()), ceil(local_pos.z()));
    std::vector<Vec3f> Corners{Vec3f(lower.x(), lower.y(), lower.z()),
                               Vec3f(lower.x(), lower.y(), upper.z()),
                               Vec3f(lower.x(), upper.y(), lower.z()),
                               Vec3f(lower.x(), upper.y(), upper.z()),
                               Vec3f(upper.x(), lower.y(), lower.z()),
                               Vec3f(upper.x(), lower.y(), upper.z()),
                               Vec3f(upper.x(), upper.y(), lower.z()),
                               Vec3f(upper.x(), upper.y(), upper.z())};
    std::vector<float> weights;
    for (auto &corner: Corners) {
        corner = grid->transform().indexToWorld(corner);
        float weight = 1;
        for (int i = 0; i < 3; i++) {
            weight *= float(fmax((1 - fabs(corner[i] - pos[i]) / dx), 0.0));
        }
        weights.push_back(weight);
    }
    float numerator = 0;
    for (int i = 0; i <= 8; ++i) {
        numerator += weights[i] * grid->tree().getValue(openvdb::Coord(Vec3i(Corners[i])));
    }
    float denominator = 0;
    for (int i = 0; i <= 8; ++i) {
        denominator += weights[i];
    }
    return numerator / denominator;
}

float vdbGrid::sampleOpacity(const float value) {
    return std::clamp(value + opacity_sampler(gen), 0.0f, 1.0f);
}

float vdbGrid::sampleEmission(const float value) {
    return std::clamp(value * 1000 + emission_sampler(gen), 0.0f, 255.0f);
}
