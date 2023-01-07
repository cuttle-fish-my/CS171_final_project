#include "geometry.h"
#include "common.h"

TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals, std::vector<int> v_index,
                           std::vector<int> n_index, AABB aabb) : vertices(std::move(vertices)),
                                                                  normals(std::move(normals)),
                                                                  v_indices(std::move(v_index)),
                                                                  n_indices(std::move(n_index)),
                                                                  aabb(aabb) {}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
    for (int i = 0; i < v_indices.size() / 3; i++) {
        Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
        Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
        Interaction temp;
        if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
            interaction = temp;
        }
    }
    return interaction.dist < RAY_DEFAULT_MAX;
}


bool
TriangleMesh::intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i &v_idx, const Vec3i &n_idx) const {
    Vec3f v0 = vertices[v_idx[0]];
    Vec3f v1 = vertices[v_idx[1]];
    Vec3f v2 = vertices[v_idx[2]];
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pVec = ray.dir().cross(v0v2);
    float det = v0v1.dot(pVec);

    float invDet = 1.0f / det;

    Vec3f tVec = ray.eye() - v0;
    float u = tVec.dot(pVec) * invDet;
    if (u < 0 || u > 1) return false;
    Vec3f qVec = tVec.cross(v0v1);
    float v = ray.dir().dot(qVec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qVec) * invDet;
    if (t < ray.t0() || t > ray.t1()) return false;

    interaction.dist = t;
    interaction.pos = ray(t);
    interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]]
                          + (1 - u - v) * normals[n_idx[0]]);
    interaction.normal.normalize();
    return true;
}
