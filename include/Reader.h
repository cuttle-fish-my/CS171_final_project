#ifndef CS171_FINAL_PROJECT_READER_H
#define CS171_FINAL_PROJECT_READER_H

#include "common.h"
#include <string>

class Reader {
public:
    Reader() = default;

    ~Reader() = default;

    static void showMetaInfo(const std::string &dir);

    static std::vector<Vec3fGrid::Ptr> readGrids(const std::string &dir);

//    load vertex and index data from a .obj file
    static void loadObj(const std::string &dir, std::vector<Vec3f> &vertices, std::vector<Vec3f> &normals,
                        std::vector<int> &v_index, std::vector<int> &n_index);
};


#endif //CS171_FINAL_PROJECT_READER_H
