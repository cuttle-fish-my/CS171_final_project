#ifndef CS171_FINAL_PROJECT_READER_H
#define CS171_FINAL_PROJECT_READER_H

#include "common.h"
#include "geometry.h"
#include <string>

class Reader {
public:
    Reader() = default;

    ~Reader() = default;

    static void showMetaInfo(const std::string &dir);

    static std::vector<Vec3fGrid::Ptr> readGrids(const std::string &dir);

//    load vertex and index data from a .obj file
    static TriangleMesh loadObj(const std::string &dir, Vec3f translation, float scale);
};


#endif //CS171_FINAL_PROJECT_READER_H
