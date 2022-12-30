#include <openvdb/openvdb.h>
#include "common.h"
#include "Reader.h"
#include "utils.h"

int main() {
//    Reader::showMetaInfo("dataset/single-res small.vdb");
//    Reader::showMetaInfo("dataset/single-res big.vdb");
//    Reader::showMetaInfo("dataset/multi-res small.vdb");
//    Reader::showMetaInfo("dataset/multi-res big.vdb");
    std::vector<Vec3fGrid::Ptr> VectorGrids = Reader::readGrids("dataset/multi-res small.vdb");
    std::vector<floatGrid::Ptr> ScalarGrids = util::convertVectorGridsToScalarGrids(VectorGrids);
    std::vector<Vec3f> vertices, normals;
    std::vector<int> v_index, n_index;
    Reader::loadObj("dataset/sphere.obj", vertices, normals, v_index, n_index);


}