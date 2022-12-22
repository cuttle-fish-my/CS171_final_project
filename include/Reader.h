#ifndef CS171_FINAL_PROJECT_READER_H
#define CS171_FINAL_PROJECT_READER_H


#include <string>
class Reader {
public:
    Reader() = default;
    ~Reader() = default;
    static void showMetaInfo(const std::string& dir);
    static std::vector<openvdb::Vec3SGrid::Ptr> readGrids(const std::string& dir);
};


#endif //CS171_FINAL_PROJECT_READER_H
