#include "integrator.h"
#include <omp.h>
#include <utility>

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
            camera->getImage()->setPixel(dx, dy, L);
        }
    }

}

Vec3f Integrator::radiance(Ray &ray, float t0, float t1) const {
    float step_size = scene->grids[0].dx;
    Vec3f src_color = Vec3f{0, 0, 0};
    float src_opacity = 0;
    auto dst_t = (float) fmax(t0 - step_size, ray.t0());
    Vec3f dst_pos, dst_color;
    float dst_opacity;
    while (dst_t < t1 - step_size) {
        dst_t += step_size;
        dst_pos = ray(dst_t);
        float dx;
        std::tie(dst_color, dst_opacity, dx) = scene->getEmissionOpacity(dst_pos);
        // Adaptive step size
        if (dx != 0) step_size = dx;
        dst_color *= step_size;
        dst_opacity = 1 - std::pow(1 - dst_opacity, step_size);
        dst_color += (1 - dst_opacity) * src_color;
        dst_opacity += (1 - dst_opacity) * src_opacity;
        src_color = dst_color;
        src_opacity = dst_opacity;
    }
    return dst_color;
}
