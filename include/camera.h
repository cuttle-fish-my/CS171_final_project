#ifndef CS171_FINAL_PROJECT_CAMERA_H
#define CS171_FINAL_PROJECT_CAMERA_H

#include <memory>
#include "ray.h"
#include "Image.h"

class Camera {
public:
    Camera();

    explicit Camera(const vec3f position, const float vertical_fov, const float focal_length,
                    std::shared_ptr<ImageRGB> &img);

    Ray generateRay(float x, float y);

    void lookAt(const vec3f &look_at, const vec3f &ref_up = {0, 1, 0});

    void setPosition(const vec3f &pos);

    [[nodiscard]] vec3f getPosition() const;

    void setFov(float new_fov);

    [[nodiscard]] float getFov() const;

    void setImage(std::shared_ptr<ImageRGB> &img);

    [[nodiscard]] std::shared_ptr<ImageRGB> &getImage();

private:
    vec3f position;
    vec3f forward;
    vec3f up;
    vec3f right;
    float focal_len;
    float fov;

    std::shared_ptr<ImageRGB> image;
};

#endif //CS171_FINAL_PROJECT_CAMERA_H
