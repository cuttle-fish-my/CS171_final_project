#ifndef CS171_FINAL_PROJECT_SCENE_H
#define CS171_FINAL_PROJECT_SCENE_H

#include <vector>
#include "common.h"
#include "AABB.h"
#include "geometry.h"

class Scene {
public:
    Scene() = default;

    std::vector<vdbGrid> grids;
    std::vector<floatGrid::Ptr> moduleGrids;
    std::vector<floatGrid::Ptr> QGrids;
    std::vector<Vec3fGrid::Ptr> vectorGrids;

    TriangleMesh sphere;
    TriangleMesh dilated_sphere;
    float min_value{};
    float max_value{};

    Vec3f lightDir{};
    Vec3f lightColor{};

    KDTree tree;

    [[nodiscard]] int searchTree(Vec3f pos) const;

    void setGrids(const std::vector<vdbGrid> &Grids);

    void setSphere(const TriangleMesh &mesh);

    void setDilatedSphere(const TriangleMesh &mesh);

    template<typename T, typename GridType>
    [[nodiscard]] int interpolation(const Vec3f &pos, T &res, const std::vector<GridType> &Grids) const;

    static float sampleOpacity(float value);

    [[nodiscard]] Vec3f sampleEmission(float value, float opacity) const;

    [[nodiscard]] std::tuple<Vec3f, float, float> getEmissionOpacity(Vec3f value) const;

    void setVectorGrids(const std::vector<Vec3fGrid::Ptr> &Grids);

    void genQGrids();

    void setLight(Vec3f dir, Vec3f color);
};

#endif //CS171_FINAL_PROJECT_SCENE_H
