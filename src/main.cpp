#include <openvdb/openvdb.h>
#include "common.h"
#include "Reader.h"
#include "utils.h"
#include "camera.h"
#include "integrator.h"
#include "scene.h"
#include "AABB.h"
#include "config.h"

int main() {
    setbuf(stdout, nullptr);
#ifdef MULTI_BIG
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/multi-res big.vdb");
#endif

#ifdef MULTI_SMALL
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/multi-res small.vdb");
#endif

#ifdef SINGLE_BIG
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/single-res big.vdb");
#endif

#ifdef SINGLE_SMALL
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/single-res small.vdb");
#endif
    std::vector<vdbGrid> ScalarGrids = util::convertVectorGridsToScalarGrids(VectorGrids);

    auto sphere = Reader::loadObj("dataset/sphere.obj", Vec3f(5, 2, 3), 4.0 / 3.0);

    auto dilated_sphere = Reader::loadObj("dataset/sphere.obj", Vec3f(5, 2, 3), 4.3 / 3.0);
#ifdef FOUR_K
    std::shared_ptr<ImageRGB> image = std::make_shared<ImageRGB>(3840, 2160);
#else
    std::shared_ptr<ImageRGB> image = std::make_shared<ImageRGB>(1920, 1080);
#endif
    Camera cam(Vec3f(5, 4, 15), 19.5, 1, Vec3f(9, 0, -10), Vec3f(0, 1, 0), image);
//    Camera cam(Vec3f(3, 15, 0), 19.5, 1, Vec3f(10,-10,0), Vec3f(0, 1, 0), image);
    Scene scene;
    scene.setGrids(ScalarGrids);
    scene.setVectorGrids(VectorGrids);
    scene.genQGrids();
    scene.setSphere(sphere);
    scene.setDilatedSphere(dilated_sphere);
    scene.setLight(Vec3f{10, -5, -2}, Vec3f{1, 1, 1});
    Integrator integrator(std::make_shared<Camera>(cam), std::make_shared<Scene>(scene));
    std::cout << "Start Rendering..." << std::endl;
    auto start = std::chrono::steady_clock::now();
    // render scene
    integrator.render();
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "\nRender Finished in " << time << "s." << std::endl;
    image->writeImgToFile("./result.png");
    std::cout << "Image saved to disk." << std::endl;
    return 0;
}