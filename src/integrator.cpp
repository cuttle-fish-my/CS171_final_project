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
    return Vec3f(0, 1, 0);
}
