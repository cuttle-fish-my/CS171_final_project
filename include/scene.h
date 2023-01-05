#ifndef CS171_FINAL_PROJECT_SCENE_H
#define CS171_FINAL_PROJECT_SCENE_H
#include <vector>
#include "common.h"
#include "AABB.h"
class Scene {
public:
    Scene() = default;

    std::vector<vdbGrid> grids;

    void setGrids(const std::vector<vdbGrid> &Grids);

    float interpolation(const Vec3f &pos) const;

    static float sampleOpacity(float value) ;

    static Vec3f sampleEmission(float value);
};

#endif //CS171_FINAL_PROJECT_SCENE_H
