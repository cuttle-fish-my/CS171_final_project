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
//    Vec3f radiance(Ray &ray, )
private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
};

#endif //CS171_FINAL_PROJECT_INTEGRATOR_H
