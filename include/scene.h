#ifndef CS171_FINAL_PROJECT_SCENE_H
#define CS171_FINAL_PROJECT_SCENE_H
#include <vector>
#include "common.h"
#include "AABB.h"
class Scene {
public:
    Scene() = default;
    std::vector<vdbGrid> grids;
//    std::vector<std::shared_ptr(Object)> objects;
    void setGrids(const std::vector<vdbGrid> &Grids);

};

#endif //CS171_FINAL_PROJECT_SCENE_H
