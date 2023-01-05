#include "scene.h"

void Scene::setGrids(const std::vector<vdbGrid> &Grids) {
    grids = Grids;
    KDTree *current = &tree;
    current->aabb = grids[0].aabb;
    for (int i = 0; i < grids.size() - 1; ++i) {
        auto [out_lo, out_hi] = grids[i].aabb;
        out_lo = grids[i].grid->transform().indexToWorld(grids[i].grid->evalActiveVoxelBoundingBox().min() - Vec3d(1));
        auto [in_lo, in_hi] = grids[i + 1].aabb;
        in_lo = grids[i + 1].grid->transform().indexToWorld(grids[i + 1].grid->evalActiveVoxelBoundingBox().min() - Vec3d(1));
        current->leaf = false;
        current->children.push_back(KDTree{.leaf=false, .aabb = AABB{in_lo, in_hi}});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{out_lo.x(), out_lo.y(), out_lo.z()},
                             {in_lo.x(),  out_hi.y(), out_hi.z()}},
                .support_layer = i});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{in_hi.x(),  out_lo.y(), out_lo.z()},
                             {out_hi.x(), out_hi.y(), out_hi.z()}},
                .support_layer = i});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{in_lo.x(), out_lo.y(), out_lo.z()},
                             {in_hi.x(), in_lo.y(),  out_hi.z()}},
                .support_layer = i});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{in_lo.x(), in_hi.y(),  out_lo.z()},
                             {in_hi.x(), out_hi.y(), out_hi.z()}},
                .support_layer = i});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{in_lo.x(), in_lo.y(), out_lo.z()},
                             {in_hi.x(), in_hi.y(), in_lo.z()}},
                .support_layer = i});
        current->children.push_back(KDTree{.leaf=true,
                .aabb = AABB{{in_lo.x(), in_hi.y(), in_hi.z()},
                             {in_hi.x(), in_hi.y(), out_hi.z()}},
                .support_layer = i});
        current = &current->children[0];
    }
    current->support_layer = static_cast<int>(grids.size()) - 1;
    current->leaf = true;
}

int Scene::searchTree(const Vec3f pos) const {
    auto recursiveFind = [](const KDTree *layer, const Vec3f pos, auto &&findLayer) {
        if (!layer->aabb.inAABB(pos)) return -1;
        if (layer->leaf) return layer->support_layer;
        for (const auto &child: layer->children) {
            if (child.aabb.inAABB(pos)) {
                return findLayer(&child, pos, findLayer);
            }
        }
        return -1;
    };
    return recursiveFind(&tree, pos, recursiveFind);
}

std::pair<float, float> Scene::interpolation(const Vec3f &pos) const {
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

    // Traverse KD-tree
    int layers = searchTree(pos);
    if (layers == -1) return {0.0f, 0.0f};

    float numerator = 0;
    float denominator = 0;
    for (int r = 0; r < 8; ++r) {
        for (int i = layers; i >= 0; --i) {
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
    if (denominator == 0) return {0.0f, grids[layers].dx};
    else return {numerator / denominator, grids[layers].dx};
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

std::tuple<Vec3f, float, float> Scene::getEmissionOpacity(Vec3f value) const {
    auto [inter, layer] = interpolation(value);
    return {sampleEmission(inter), sampleOpacity(inter), layer};
}
