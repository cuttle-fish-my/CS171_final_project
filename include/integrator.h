#ifndef CS171_FINAL_PROJECT_INTEGRATOR_H
#define CS171_FINAL_PROJECT_INTEGRATOR_H

#include "common.h"
#include "utils.h"
#include "camera.h"
#include "scene.h"

class Integrator {
public:
    Integrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene);

    void render() const;

    std::pair<Vec3f, float> radiance(Ray &ray, float t0, float t1 = 0.0f) const;

    Vec3f radiance(Ray &ray, Interaction &interaction, Vec3f objColor = Vec3f{1, 1, 1}) const;

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
};

#endif //CS171_FINAL_PROJECT_INTEGRATOR_H
