#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>
#include "Image.h"
#include "utils.h"

ImageRGB::ImageRGB(int width, int height) : resolution(width, height) {
    data.resize(width * height);
}

float ImageRGB::getAspectRatio() const {
    return static_cast<float> (resolution.x()) / static_cast<float> (resolution.y());
}

Vec2i ImageRGB::getResolution() const {
    return resolution;
}

void ImageRGB::setPixel(int x, int y, const Vec3f &value) {
    data[y * resolution.x() + x] = value;
}

void ImageRGB::writeImgToFile(const std::string &file_name) {
    std::vector<uint8_t> rgb_data(resolution.x() * resolution.y() * 3);
    for (int i = 0; i < data.size(); i++) {
        rgb_data[3 * i] =  util::gammaCorrection(data[i].x());
        rgb_data[3 * i + 1] = util::gammaCorrection(data[i].y());
        rgb_data[3 * i + 2] = util::gammaCorrection(data[i].z());
    }

    stbi_flip_vertically_on_write(true);
    stbi_write_png(file_name.c_str(), resolution.x(), resolution.y(), 3, rgb_data.data(), 0);
}

