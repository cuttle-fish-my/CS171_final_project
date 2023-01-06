#include "scene.h"
#include "utils.h"

void Scene::setGrids(const std::vector<vdbGrid> &Grids) {
    grids = Grids;
    for (auto &grid: Grids) {
        moduleGrids.push_back(grid.grid);
    }
    KDTree *current = &tree;
    current->aabb = grids[0].aabb;
    for (int i = 0; i < grids.size() - 1; ++i) {
        auto [out_lo, out_hi] = grids[i].aabb;
        out_lo = grids[i].grid->transform().indexToWorld(grids[i].grid->evalActiveVoxelBoundingBox().min() - Vec3d(1));
        auto [in_lo, in_hi] = grids[i + 1].aabb;
        in_lo = grids[i + 1].grid->transform().indexToWorld(
                grids[i + 1].grid->evalActiveVoxelBoundingBox().min() - Vec3d(1));
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

template<typename T, typename GridType>
float Scene::interpolation(const Vec3f &pos, T &res, const std::vector<GridType> &Grids) const {
    std::vector<std::vector<Vec3i>> Corners;
    for (const auto &grid: Grids) {
        Vec3d local_pos = (grid->transform().worldToIndex(pos));
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

    res = T(0);

    // Traverse KD-tree
    int layers = searchTree(pos);
    if (layers == -1) return 0.0f;

    T numerator(0);

    float denominator = 0;
    for (int r = 0; r < 8; ++r) {
        for (int i = layers; i >= 0; --i) {
            openvdb::Coord coord{Corners[i][r]};
            float dx = float(Grids[i]->transform().voxelSize()[0]);
            if (Grids[i]->tree().isValueOn(coord)) {
                Vec3f corner = Grids[i]->transform().indexToWorld(Corners[i][r]);
                float weight = 1;
                for (int j = 0; j < 3; j++) {
                    weight *= float(fmax((1 - fabs(corner[j] - pos[j]) / dx), 0.0));
                }
                numerator += weight * Grids[i]->tree().getValue(coord);
                denominator += weight;
                break;
            }
        }
    }
    if (denominator != 0) res = numerator / denominator;
    return float(Grids[layers]->transform().voxelSize()[0]);
}

float Scene::sampleOpacity(float value) {
//    float opacity = std::exp(-std::abs(value - 0.05f) * 100);
//    return opacity;

    float isoValue = 0.05f;
    float margin = 0.035f;
    if (value < isoValue + margin && value > isoValue - margin) {
//        return 1 - std::abs(value - isoValue) / margin;
        return std::exp(-std::abs(value - 0.05f) * 30);
    } else {
        return 0;
    }
//    else return 0.0f;
}

Vec3f Scene::sampleEmission(float value) {
    float opacity = sampleOpacity(value);
    return Vec3f{
            std::exp(-(opacity - 1) * (opacity - 1) * 100),
            0,
            0
    };
//    Vec3f color = Vec3f{0, 0, 0};
//    if (value > 0.001 && value <= 0.005) {
//        color = Vec3f{
//                0,
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0025f) * 100),
//                0.33f * 0.5f * std::exp(-std::abs(value - 0.0025f) * 100)
//        };
//    } else if (value > 0.005 && value <= 0.01) {
//        color = Vec3f{
//                0,
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0075f) * 100),
//                0.66f * 0.5f * std::exp(-std::abs(value - 0.0075f) * 100)
//        };
//    } else if (value > 0.015 && value <= 0.02) {
//        color = Vec3f{
//                0,
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0175f) * 100),
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0175f) * 100)
//        };
//    } else if (value > 0.02 && value <= 0.025) {
//        color = Vec3f{
//                0,
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0225f) * 100),
//                0.33f * 0.5f * std::exp(-std::abs(value - 0.0225f) * 100)
//        };
//    } else if (value > 0.025 && value <= 0.03) {
//        color = Vec3f{
//                0,
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0275f) * 100),
//                0.66f * 0.5f * std::exp(-std::abs(value - 0.0275f) * 100)
//        };
//    } else if (value > 0.03 && value <= 0.035) {
//        color = Vec3f{
//                0,
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0325f) * 100),
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0325f) * 100)
//        };
//    } else if (value > 0.035 && value <= 0.04) {
//        color = Vec3f{
//                0,
//                0.33f * 0.5f * std::exp(-std::abs(value - 0.0375f) * 100),
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0375f) * 100)
//        };
//    } else if (value > 0.04 && value <= 0.045) {
//        color = Vec3f{
//                0,
//                0.66f * 0.5f * std::exp(-std::abs(value - 0.0425f) * 100),
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0425f) * 100)
//        };
//    } else if (value > 0.045 && value <= 0.05) {
//        color = Vec3f{
//                0,
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0475f) * 100),
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0475f) * 100)
//        };
//    } else if (value > 0.05 && value <= 0.055) {
//        color = Vec3f{
//                0,
//                0.33f * 0.5f * std::exp(-std::abs(value - 0.0525f) * 100),
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0525f) * 100)
//        };
//    } else if (value > 0.055 && value <= 0.06) {
//        color = Vec3f{
//                0,
//                0.66f * 0.5f * std::exp(-std::abs(value - 0.0575f) * 100),
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0575f) * 100)
//        };
//    } else if (value > 0.06 && value <= 0.065) {
//        color = Vec3f{
//                0,
//                1.0f * 0.5f * std::exp(-std::abs(value - 0.0625f) * 100),
//                0.5f * 0.5f * std::exp(-std::abs(value - 0.0625f) * 100)
//        };
//    }
//    return color;
}

std::tuple<Vec3f, float, float> Scene::getEmissionOpacity(Vec3f value) const {
    float emission_inter, opacity_inter;
    auto layer = interpolation(value, emission_inter, moduleGrids);
    layer = interpolation(value, opacity_inter, QGrids);
    auto opacity = sampleOpacity(opacity_inter);
    auto color = 2 * Vec3f(opacity);
//    sampleEmission(emission_inter)
    return {color, opacity, layer};
}

void Scene::setVectorGrids(const std::vector<Vec3fGrid::Ptr> &Grids) {
    vectorGrids = Grids;
}

void Scene::genQGrids() {
    for (const auto &grid: vectorGrids) {
        float dx = (float) (grid->transform().voxelSize()[0]);
        auto qGrid = floatGrid::create();
        qGrid->setTransform(grid->transform().copy());
        qGrid->topologyUnion(*grid);
        auto accessor = qGrid->getAccessor();
        auto vec_accessor = grid->getAccessor();
        for (auto iter = grid->cbeginValueOn(); iter; ++iter) {
            float qValue = 0.0f;
            openvdb::Coord ijk = iter.getCoord();
            openvdb::Coord low_x = ijk + openvdb::Coord{-1, 0, 0}, low_y = ijk + openvdb::Coord{0, -1, 0},
                    low_z = ijk + openvdb::Coord{0, 0, -1};
            openvdb::Coord high_x = ijk + openvdb::Coord{1, 0, 0}, high_y = ijk + openvdb::Coord{0, 1, 0},
                    high_z = ijk + openvdb::Coord{0, 0, 1};
            Vec3f grad_x = (vec_accessor.getValue(high_x) - vec_accessor.getValue(low_x)) / (2 * dx);
            Vec3f grad_y = (vec_accessor.getValue(high_y) - vec_accessor.getValue(low_y)) / (2 * dx);
            Vec3f grad_z = (vec_accessor.getValue(high_z) - vec_accessor.getValue(low_z)) / (2 * dx);

            qValue = -0.5f * (grad_x[0] * grad_x[0] + grad_y[1] * grad_y[1] + grad_z[2] * grad_z[2]) -
                     grad_y[0] * grad_x[1] - grad_z[0] * grad_x[2] - grad_z[1] * grad_y[2];
//            std::cout<<qValue<<std::endl;
            accessor.setValue(iter.getCoord(), qValue);
        }
        QGrids.push_back(qGrid);
    }
//    for (const auto &grid: QGrids) {
//        float m, mm;
//        grid->evalMinMax(m, mm);
//        std::cout << m << " " << mm << std::endl;
//    }

}
