#include "camera.h"

Camera::Camera(const Vec3f pos, const float vertical_fov, const float focal_length, const Vec3f look_at,
               const Vec3f ref_up, std::shared_ptr<ImageRGB> &img) : position(pos), fov(vertical_fov),
                                                                     focal_len(focal_length) {
    image = img;
    lookAt(look_at, ref_up);
}

Ray Camera::generateRay(const float x, const float y) {
    const auto resolution = image->getResolution();
    float ratio = image->getAspectRatio();
    const auto dx = (2 * x / static_cast<float>(resolution.x()) - 1) * focal_len * ratio *
        std::tan(getFov() * PI / 180.f / 2);
    const auto dy = (2 * y / static_cast<float>(resolution.y()) - 1) * focal_len *
        std::tan(getFov() * PI / 180.f / 2);
    Vec3f dir = (dx * right + dy * up + forward);
    dir.normalize();
    return Ray{position, dir, RAY_DEFAULT_MIN, RAY_DEFAULT_MAX};
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    forward = look_at - position;
    forward.normalize();
    right = forward.cross(ref_up);
    right.normalize();
    up = right.cross(forward);
    up.normalize();
}

void Camera::setPosition(const Vec3f &pos) {
    position = pos;
}

Vec3f Camera::getPosition() const {
    return position;
}

void Camera::setFov(const float new_fov) {
    fov = new_fov;
}

float Camera::getFov() const {
    return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
    image = img;
}

std::shared_ptr<ImageRGB> &Camera::getImage() {
    return image;
}

