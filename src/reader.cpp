#include <openvdb/openvdb.h>
#include "Reader.h"

void Reader::showMetaInfo(const std::string &dir) {
    openvdb::initialize();
    openvdb::io::File file(dir);
    file.open();
    openvdb::GridBase::Ptr baseGrid;
    std::cout << "------------- start of a vdb -------------" << std::endl;
    for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
        baseGrid = file.readGrid(nameIter.gridName());
        std::cout << "--------- start of a grid ---------" << std::endl;
        std::cout << "Grid Type: " << baseGrid->type() << std::endl;
        for (auto iter = baseGrid->beginMeta(); iter != baseGrid->endMeta(); ++iter) {
            const std::string &name = iter->first;
            openvdb::Metadata::Ptr value = iter->second;
            std::string valueAsString = value->str();

            std::cout << '(' << value->typeName() << ") " << name << " = " << valueAsString << std::endl;
        }
        std::cout << "--------- end of a grid ---------" << std::endl;
    }
    std::cout << "------------- end of a vdb -------------" << std::endl;
    std::cout<<std::endl;
    file.close();
}

std::vector<openvdb::Vec3SGrid::Ptr> Reader::readGrids(const std::string &dir) {
    openvdb::initialize();
    openvdb::io::File file(dir);
    file.open();
    openvdb::GridBase::Ptr baseGrid;
    std::vector<openvdb::Vec3SGrid::Ptr> grids;
    for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
        baseGrid = file.readGrid(nameIter.gridName());
        grids.push_back(openvdb::gridPtrCast<openvdb::Vec3SGrid>(baseGrid));
    }
    file.close();
    return grids;
}

