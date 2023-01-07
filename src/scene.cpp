#include "scene.h"

float isoValue = 0.05f;
float margin = 0.04f;

void Scene::setGrids(const std::vector<vdbGrid> &Grids) {
    grids = Grids;

    min_value = std::min(grids[0].min_value, std::min(grids[1].min_value, grids[2].min_value));
    max_value = std::max(grids[0].max_value, std::max(grids[1].max_value, grids[2].max_value));

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
    if (value < isoValue + margin && value > isoValue) {
        return std::exp(-std::abs(value - 0.05f));
    } else {
        return 0;
    }
}

Vec3f Scene::sampleEmission(float value, float opacity) const {
    float a_coef = 0, b_coef = 0, c_coef = 0, d_coef = 0;
    float mid_value_1 = (max_value + min_value) / 3;
    float mid_value_2 = 2 * mid_value_1;
    Vec3f A{0, 0.9, 0.2}, B{0.8, 0.8, 0}, C{1, 0, 0}, D{0.4, 0.6, 0};
    if (value < mid_value_1) {
        float range = mid_value_1 - min_value;
        a_coef = 1 - (value - min_value) / range;
        b_coef = 1 - (mid_value_1 - value) / range;
    } else if (value >= mid_value_1 && value < mid_value_2){
        float range = mid_value_2 - mid_value_1;
        b_coef = 1 - (value - mid_value_1) / range;
        c_coef = 1 - (mid_value_2 - value) / range;
    } else {
        float range = max_value - mid_value_2;
        c_coef = 1 - (value - mid_value_2) / range;
        d_coef = 1 - (max_value - value) / range;
    }

    auto color = a_coef * A + b_coef * B + c_coef * C + d_coef * D;
    if (opacity < 1e-3) {
        return {0, 0, 0};
    } else {
        return 10 * color;
    }

//    return 2 * (a_coef * A + b_coef * B + c_coef * C + d_coef * D) * opacity;
}

std::tuple<Vec3f, float, float> Scene::getEmissionOpacity(Vec3f value) const {
    float emission_inter, opacity_inter;
    auto layer = interpolation(value, emission_inter, moduleGrids);
    layer = interpolation(value, opacity_inter, QGrids);
    auto opacity = sampleOpacity(opacity_inter);
    auto color = sampleEmission(emission_inter, opacity);
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
            accessor.setValue(iter.getCoord(), qValue);
        }
        QGrids.push_back(qGrid);
    }
}

void Scene::setSphere(const TriangleMesh &mesh) {
    sphere = mesh;
}
