#include <openvdb/openvdb.h>
#include "Reader.h"


int main() {
//    Reader::showMetaInfo("dataset/single-res small.vdb");
//    Reader::showMetaInfo("dataset/single-res big.vdb");
//    Reader::showMetaInfo("dataset/multi-res small.vdb");
//    Reader::showMetaInfo("dataset/multi-res big.vdb");
//    Reader::readGrids("dataset/multi-res small.vdb");
    std::vector<Vec3f> vertices, normals;
    std::vector<int> v_index, n_index;
    Reader::loadObj("dataset/sphere.obj", vertices, normals, v_index, n_index);


}