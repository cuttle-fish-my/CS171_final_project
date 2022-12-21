#include <openvdb/openvdb.h>

int main() {
    openvdb::initialize();
    openvdb::io::File file("dataset/single-res big.vdb");
    file.open();
    openvdb::GridBase::Ptr baseGrid;
    for (openvdb::io::File::NameIterator nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
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