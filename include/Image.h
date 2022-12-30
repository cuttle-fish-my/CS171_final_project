#ifndef CS171_FINAL_PROJECT_IMAGE_H
#define CS171_FINAL_PROJECT_IMAGE_H
#include <vector>
#include <string>

#include "common.h"

class ImageRGB {
public:
    ImageRGB() = delete;

    ImageRGB(int width, int height);

    [[nodiscard]] float getAspectRatio() const;

    [[nodiscard]] vec2i getResolution() const;

    void setPixel(int x, int y, const vec3f &value);

    void writeImgToFile(const std::string &file_name);

private:
    std::vector<vec3f> data;
    vec2i resolution;
};
#endif //CS171_FINAL_PROJECT_IMAGE_H
