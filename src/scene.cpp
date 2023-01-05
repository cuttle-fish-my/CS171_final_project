#include "scene.h"

void Scene::setGrids(const std::vector<vdbGrid> &Grids) {
    grids = Grids;
}

float Scene::interpolation(const Vec3f &pos) const {
    std::vector<std::vector<Vec3i>> Corners;

    for (const auto &grid: grids) {
        Vec3d local_pos = (grid.grid->transform().worldToIndex(pos));
        Vec3i lower = {Vec3i(floor(local_pos.x()), floor(local_pos.y()), floor(local_pos.z()))};
        Vec3i upper = {Vec3i(ceil(local_pos.x()), ceil(local_pos.y()), ceil(local_pos.z()))};
        Corners.push_back({{lower.x(), lower.y(), lower.z()},
                           {lower.x(), lower.y(), upper.z()},
                           {lower.x(), upper.y(), lower.z()},
                           {lower.x(), upper.y(), upper.z()},
                           {upper.x(), lower.y(), lower.z()},
                           {upper.x(), lower.y(), upper.z()},
                           {upper.x(), upper.y(), lower.z()},
                           {upper.x(), upper.y(), upper.z()}});
    }

    float numerator = 0;
    float denominator = 0;
    for (int r = 0; r < 8; ++r) {
        for (int i = static_cast<int>(Corners.size()) - 1; i >= 0; --i) {
            openvdb::Coord coord{Corners[i][r]};
            if (grids[i].grid->tree().isValueOn(coord)) {
                Vec3f corner = grids[i].grid->transform().indexToWorld(Corners[i][r]);
                float weight = 1;
                for (int j = 0; j < 3; j++) {
                    weight *= float(fmax((1 - fabs(corner[j] - pos[j]) / grids[i].dx), 0.0));
                }
                numerator += weight * grids[i].grid->tree().getValue(coord);
                denominator += weight;
                break;
            }
        }
    }
    return denominator == 0 ? 0 : numerator / denominator;
}

float Scene::sampleOpacity(float value) {
    float opacity = 0.5f * std::exp(-std::abs(value - 0.03f) * 100);
    return opacity;
}

Vec3f Scene::sampleEmission(float value) {
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

std::pair<Vec3f, float> Scene::getEmissionOpacity(Vec3f value) const {
    auto inter = interpolation(value);
    return {sampleEmission(inter), sampleOpacity(inter)};
}
