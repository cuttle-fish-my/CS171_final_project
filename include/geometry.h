#ifndef CS171_FINAL_PROJECT_GEOMETRY_H
#define CS171_FINAL_PROJECT_GEOMETRY_H

#include "common.h"
#include "interaction.h"

class TriangleMesh {
public:
    TriangleMesh() = default;

    TriangleMesh(std::vector<Vec3f> vertices,
                 std::vector<Vec3f> normals,
                 std::vector<int> v_index,
                 std::vector<int> n_index);

    bool intersect(Ray &ray, Interaction &interaction) const;


    bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i &v_idx, const Vec3i &n_idx) const;
    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<int> v_indices;
    std::vector<int> n_indices;
};


#endif //CS171_FINAL_PROJECT_GEOMETRY_H
