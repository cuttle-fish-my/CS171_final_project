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
    Vec3f src_color = Vec3f{0, 0, 0};
    float src_opacity = 0;
    auto dst_t = (float) fmax(t0 - step_size, ray.t0());
    Vec3f dst_pos, dst_color;
    float dst_opacity;
    float dx;
    while (dst_t < t1 - step_size) {
        dst_t += step_size;
        dst_pos = ray(dst_t);
        std::tie(dst_color, dst_opacity, dx) = scene->getEmissionOpacity(dst_pos);
        if (dx != 0) step_size = dx / 2;

        dst_color *= step_size;
        dst_opacity = 1 - std::pow(1 - dst_opacity, step_size);

        dst_color += (1 - dst_opacity) * src_color;
        dst_opacity += (1 - dst_opacity) * src_opacity;

        src_color = dst_color;
        src_opacity = dst_opacity;
    }
    return dst_color;
}

Vec3f Integrator::radiance(Ray &ray, Interaction &interaction) const {
    Vec3f radiance(0, 0, 0);
    Vec3f lightDir{10, -5, 10};
    Vec3f lightColor{1, 1, 1};
    lightDir.normalize();
    Vec3f ambient{0.1, 0.1, 0.1};
    Vec3f diffuse{0, 0, 0};
    Vec3f specular{0, 0, 0};
    Ray shadow_ray{interaction.pos, -lightDir};
//    shadow_ray.t_max = (sample.position - interaction.pos).norm() / shadow_ray.direction.norm();
    Interaction new_interaction{};
    if (!scene->sphere.intersect(shadow_ray, new_interaction)) {
        lightDir = -lightDir;
        Vec3f viewDir = -ray.dir();
        float diff = std::max(interaction.normal.dot(lightDir), 0.0f);
//        set material of sphere to white.
        diffuse += diff * Vec3f{1.0f, 1.0f, 1.0f};
        if (lightDir.dot(interaction.normal) > 0) {
            Vec3f reflectDir = -lightDir - 2.0f * interaction.normal.dot(-lightDir) * interaction.normal;
            float spec = (float) std::pow(std::max(viewDir.dot(reflectDir), 0.0f), 32);
            specular += spec * Vec3f{1.0f, 1.0f, 1.0f};
        }
    }

    radiance = (diffuse + specular + ambient) * lightColor;
    return radiance;
}
