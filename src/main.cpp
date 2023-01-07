#include <openvdb/openvdb.h>
#include "common.h"
#include "Reader.h"
#include "utils.h"
#include "camera.h"
#include "integrator.h"
#include "scene.h"
#include "AABB.h"

int main() {
    setbuf(stdout, nullptr);
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/multi-res big.vdb");
    std::vector<vdbGrid> ScalarGrids = util::convertVectorGridsToScalarGrids(VectorGrids);

    std::shared_ptr<ImageRGB> image = std::make_shared<ImageRGB>(3840, 2160);
    Camera cam(Vec3f(3, 3.2, 15), 19.5, 1, Vec3f(10, 0, -10), Vec3f(0, 1, 0), image);
    Scene scene;
    scene.setGrids(ScalarGrids);
    scene.setVectorGrids(VectorGrids);
    scene.genQGrids();
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