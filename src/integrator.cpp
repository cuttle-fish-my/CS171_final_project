#include "integrator.h"
#include <omp.h>
#include <utility>
#include "interaction.h"

Integrator::Integrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene) : camera(std::move(cam)),
                                                                                    scene(std::move(scene)) {
}

void Integrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    for (auto &grid: scene->grids) {
        std::cout << grid.aabb.lower_bnd << " " << grid.aabb.upper_bnd << std::endl;
    }
    int cnt = 0;
#pragma omp parallel for schedule(guided, 2), default(none), shared(cnt, resolution)
    for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0, 0, 0);
            Ray ray = camera->generateRay((float) dx, (float) dy);
            float t0, t1;
            if (scene->grids[0].aabb.intersect(ray, t0, t1)) {
                L += radiance(ray, t0, t1);
            }
            Interaction interaction;
            if (scene->sphere.aabb.intersect(ray, t0, t1)) {
                if (scene->sphere.intersect(ray, interaction)) {
                    L += radiance(ray, interaction);
                }
            }
            camera->getImage()->setPixel(dx, dy, L);
        }
    }

}

Vec3f Integrator::radiance(Ray &ray, float t0, float t1) const {
    float step_size = scene->grids[0].dx / 2;
    Vec3f src_color;
    float src_opacity;
    auto src_t = (float) fmax(t0 - step_size, ray.t0());
    Vec3f src_pos, dst_color = Vec3f{0, 0, 0};
    float dst_opacity = 0;
    float dx;
    int layer;
    auto accessor = scene->moduleGrids[0]->getAccessor();
    bool flag = false;
    while (src_t < t1 - step_size) {
        src_t += step_size;
        src_pos = ray(src_t);
        std::tie(src_color, src_opacity, layer) = scene->getEmissionOpacity(src_pos);

        if (layer != -1) {
            step_size = scene->grids[layer].dx / 2;
            accessor = scene->moduleGrids[layer]->getAccessor();
        }
        src_opacity = (float) (1.0 - std::pow(1 - src_opacity, step_size));
        dst_color += (1 - dst_opacity) * src_color;
        dst_opacity += (1 - dst_opacity) * src_opacity;
        if (src_opacity != 0 && !flag) {
            if (layer == -1) {
                std::cerr << "Error: src_opacity != 0 but layer == -1" << std::endl;
            }
            openvdb::Coord ijk(Vec3i(scene->moduleGrids[layer]->transform().worldToIndex(src_pos)));
            openvdb::Coord low_x = ijk + openvdb::Coord{-1, 0, 0}, low_y = ijk + openvdb::Coord{0, -1, 0},
                    low_z = ijk + openvdb::Coord{0, 0, -1};
            openvdb::Coord high_x = ijk + openvdb::Coord{1, 0, 0}, high_y = ijk + openvdb::Coord{0, 1, 0},
                    high_z = ijk + openvdb::Coord{0, 0, 1};
            float grad_x = (accessor.getValue(high_x) - accessor.getValue(low_x)) / (2 * dx);
            float grad_y = (accessor.getValue(high_y) - accessor.getValue(low_y)) / (2 * dx);
            float grad_z = (accessor.getValue(high_z) - accessor.getValue(low_z)) / (2 * dx);
            Vec3f normal = Vec3f{grad_x, grad_y, grad_z};
            normal.normalize();
            Interaction interaction{src_pos, -1, normal};
            dst_color += radiance(ray, interaction, src_color);
            flag = true;
        }
    }
    return dst_color;
}

Vec3f Integrator::radiance(Ray &ray, Interaction &interaction, Vec3f objColor) const {
    Vec3f radiance;
    Vec3f ambient{0.1, 0.1, 0.1};
    Vec3f diffuse{0, 0, 0};
    Vec3f specular{0, 0, 0};
    Ray shadow_ray{interaction.pos, -scene->lightDir};
    Interaction new_interaction{};
    if (interaction.dist > 0 && !scene->sphere.intersect(shadow_ray, new_interaction)) {
        auto lightDir = -scene->lightDir;
        Vec3f viewDir = -ray.dir();
        float diff = std::max(interaction.normal.dot(lightDir), 0.0f);
//        set material of sphere to white.
        diffuse += diff * objColor;
        if (lightDir.dot(interaction.normal) > 0) {
            Vec3f reflectDir = -lightDir - 2.0f * interaction.normal.dot(-lightDir) * interaction.normal;
            float spec = (float) std::pow(std::max(viewDir.dot(reflectDir), 0.0f), 64);
            specular += spec * objColor;
        }
    }

    radiance = (diffuse + specular + ambient) * scene->lightColor;
    return radiance;
}
