#include "integrator.h"
#include <omp.h>
#include <utility>

Integrator::Integrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene) : camera(std::move(cam)),
                                                                                    scene(std::move(scene)) {
}

void Integrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
#pragma omp parallel for schedule(guided, 2), default(none), shared(cnt, resolution)
    for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0, 0, 0);
            Ray ray = camera->generateRay((float) dx, (float) dy);
            for (const auto &grid: scene->grids) {
                float t0, t1;
                if (grid.aabb.intersect(ray, t0, t1)) {
                    L += radiance(ray, t0, t1);
                }
            }
            camera->getImage()->setPixel(dx, dy, L);
        }
    }

}

Vec3f Integrator::radiance(Ray &ray, float t0, float t1) const {
    float step_size = 0.5;
//    Vec3f src_pos = ray(src_t);
//    Vec3f src_color = scene->grids[0].sampleEmission(scene->grids[0].interpolation(src_pos));
//    float src_opacity = scene->grids[0].sampleOpacity(scene->grids[0].interpolation(src_pos));
    Vec3f src_color = Vec3f{0, 0, 0};
    float src_opacity = 0;
    float dst_t = t0 - step_size;
    Vec3f dst_pos, dst_color;
    float dst_opacity;
    while (dst_t < t1) {
        dst_t += step_size;
        dst_pos = ray(dst_t);
        dst_color = scene->grids[0].sampleEmission(scene->grids[0].interpolation(dst_pos));
        dst_opacity = scene->grids[0].sampleOpacity(scene->grids[0].interpolation(dst_pos));
//        dst_opacity = 1 - std::pow(1 - dst_opacity, 0.01);
        dst_color += (1 - dst_opacity) * src_color;
        dst_opacity += (1 - dst_opacity) * src_opacity;
        src_color = dst_color;
        src_opacity = dst_opacity;
    }
    return dst_color;
}
