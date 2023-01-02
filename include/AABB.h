#ifndef CS171_FINAL_PROJECT_AABB_H
#define CS171_FINAL_PROJECT_AABB_H

#include "common.h"

struct AABB {
    Vec3f lower_bnd;
    Vec3f upper_bnd;

    explicit AABB(const floatGrid::Ptr &grid);

    bool intersect(const Ray &ray, float &t0, float &t1) const;
};

struct vdbGrid {
    floatGrid::Ptr grid;
    AABB aabb;
    float dx;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<float> opacity_sampler{0, 0.01};
    std::normal_distribution<float> emission_sampler{0, 10};


    explicit vdbGrid(const floatGrid::Ptr &grid) : grid(grid), aabb(grid) {
        dx = float(grid->transform().voxelSize()[0]);
//        print all value in grid

        for (auto iter = grid->cbeginValueOn(); iter; ++iter) {
            std::cout << iter.getValue() << std::endl;
        }
    };

    [[nodiscard]] float interpolation(const Vec3f &pos) const {
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

    float sampleOpacity(const float value) {
        return std::clamp(value + opacity_sampler(gen), 0.0f, 1.0f);
    }

    float sampleEmission(const float value) {
        return std::clamp(value * 1000 + emission_sampler(gen), 0.0f, 255.0f);
    }


};


#endif //CS171_FINAL_PROJECT_AABB_H
