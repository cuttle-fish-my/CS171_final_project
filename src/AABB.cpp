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
    for (int i = 0; i < 8; ++i) {
        openvdb::Coord coord{openvdb::Coord(Vec3i(Corners[i]))};
        if (grid->tree().isValueOn(coord)) {
            Vec3f corner = grid->transform().indexToWorld(Corners[i]);
            float weight = 1;
            for (int j = 0; j < 3; j++) {
                weight *= float(fmax((1 - fabs(corner[j] - pos[j]) / dx), 0.0));
            }
            weights.push_back(weight);
        }
    }
    float numerator = 0;
    for (int i = 0; i < weights.size(); ++i) {
        openvdb::Coord coord{openvdb::Coord(Vec3i(Corners[i]))};
        numerator += weights[i] * grid->tree().getValue(coord);
    }
    float denominator = 0;
    for (float weight: weights) {
        denominator += weight;
    }
    return weights.empty() ? 0 : numerator / denominator;
}

float vdbGrid::sampleOpacity(float value) const {
    float opacity = 0.5f * std::exp(-std::abs(value - 0.03f) * 100);
    return opacity;
}

Vec3f vdbGrid::sampleEmission(const float value) {
    Vec3f color = Vec3f{0, 0, 0};
    if (value > 0.01 && value <= 0.005) {
        color = Vec3f{
                0,
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0025f) * 100),
                0.33f * 0.5f * std::exp(-std::abs(value - 0.0025f) * 100)
        };
    } else if (value > 0.005 && value <= 0.01) {
        color = Vec3f{
                0,
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0075f) * 100),
                0.66f * 0.5f * std::exp(-std::abs(value - 0.0075f) * 100)
        };
    } else if (value > 0.015 && value <= 0.02) {
        color = Vec3f{
                0,
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0175f) * 100),
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0175f) * 100)
        };
    } else if (value > 0.02 && value <= 0.025) {
        color = Vec3f{
                0,
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0225f) * 100),
                0.33f * 0.5f * std::exp(-std::abs(value - 0.0225f) * 100)
        };
    } else if (value > 0.025 && value <= 0.03) {
        color = Vec3f{
                0,
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0275f) * 100),
                0.66f * 0.5f * std::exp(-std::abs(value - 0.0275f) * 100)
        };
    } else if (value > 0.03 && value <= 0.035) {
        color = Vec3f{
                0,
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0325f) * 100),
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0325f) * 100)
        };
    } else if (value > 0.035 && value <= 0.04) {
        color = Vec3f{
                0,
                0.33f * 0.5f * std::exp(-std::abs(value - 0.0375f) * 100),
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0375f) * 100)
        };
    } else if (value > 0.04 && value <= 0.045) {
        color = Vec3f{
                0,
                0.66f * 0.5f * std::exp(-std::abs(value - 0.0425f) * 100),
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0425f) * 100)
        };
    } else if (value > 0.045 && value <= 0.05) {
        color = Vec3f{
                0,
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0475f) * 100),
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0475f) * 100)
        };
    } else if (value > 0.05 && value <= 0.055) {
        color = Vec3f{
                0,
                0.33f * 0.5f * std::exp(-std::abs(value - 0.0525f) * 100),
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0525f) * 100)
        };
    } else if (value > 0.055 && value <= 0.06) {
        color = Vec3f{
                0,
                0.66f * 0.5f * std::exp(-std::abs(value - 0.0575f) * 100),
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0575f) * 100)
        };
    } else if (value > 0.06 && value <= 0.065) {
        color = Vec3f{
                0,
                1.0f * 0.5f * std::exp(-std::abs(value - 0.0625f) * 100),
                0.5f * 0.5f * std::exp(-std::abs(value - 0.0625f) * 100)
        };
    }
    return color;
}
