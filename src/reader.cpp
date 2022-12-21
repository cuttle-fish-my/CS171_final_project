#include <openvdb/openvdb.h>
#include "Reader.h"

void Reader::showMetaInfo(const std::string& dir) {
    openvdb::initialize();
    openvdb::io::File file(dir);
    file.open();
    openvdb::GridBase::Ptr baseGrid;
    for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
        baseGrid = file.readGrid(nameIter.gridName());
        for (auto iter = baseGrid->beginMeta(); iter != baseGrid->endMeta(); ++iter) {
            const std::string &name = iter->first;
            openvdb::Metadata::Ptr value = iter->second;
            std::string valueAsString = value->str();
            std::cout << name << " = " << valueAsString << std::endl;
        }
        std::cout<< "---------------------" <<std::endl;
    }
    file.close();
}

