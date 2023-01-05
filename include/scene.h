#ifndef CS171_FINAL_PROJECT_SCENE_H
#define CS171_FINAL_PROJECT_SCENE_H
#include <vector>
#include "common.h"
#include "AABB.h"
class Scene {
public:
    Scene() = default;

    std::vector<vdbGrid> grids;

    KDTree tree;

    [[nodiscard]] int searchTree(Vec3f pos) const;

    void setGrids(const std::vector<vdbGrid> &Grids);

    [[nodiscard]] std::pair<float, float> interpolation(const Vec3f &pos) const;

    static float sampleOpacity(float value) ;

    static Vec3f sampleEmission(float value);

    [[nodiscard]] std::tuple<Vec3f, float, float> getEmissionOpacity(Vec3f value) const;
};

#endif //CS171_FINAL_PROJECT_SCENE_H
