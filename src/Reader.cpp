#include <openvdb/openvdb.h>
#include "Reader.h"
#include <fstream>
#include <sstream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>


void Reader::showMetaInfo(const std::string &dir) {
    openvdb::initialize();
    openvdb::io::File file(dir);
    file.open();
    openvdb::GridBase::Ptr baseGrid;
    for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
        baseGrid = file.readGrid(nameIter.gridName());
        std::cout << "Grid Type: " << baseGrid->type() << std::endl;
        for (auto iter = baseGrid->beginMeta(); iter != baseGrid->endMeta(); ++iter) {
            const std::string &name = iter->first;
            openvdb::Metadata::Ptr value = iter->second;
            std::string valueAsString = value->str();

            std::cout << '(' << value->typeName() << ") " << name << " = " << valueAsString << std::endl;
        }
        std::cout << "------------------" << std::endl;
    }
    std::cout << "------------- end of a vdb -------------" << std::endl;
    std::cout << std::endl;
    file.close();
}

std::vector<openvdb::Vec3SGrid::Ptr> Reader::readGrids(const std::string &dir) {
    /* Function of reading grids from a vdb file
     * @param dir: the directory of the vdb file
     * Structure of a vdb file:
     *  - a vdb file contains multiple grids
     *  - each grid is actually a 4 layer tree:
     *      - root layer
     *      - internal layer
     *      - internal layer
     *      - leaf layer
     * */
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

void Reader::loadObj(const std::string &dir, std::vector<vec3f> &vertices, std::vector<vec3f> &normals,
                     std::vector<int> &v_index, std::vector<int> &n_index) {
    /* Function of loading vertex and index data from a .obj file
     * @param dir: the directory of the .obj file
     * @param vertices: the vector of vertices
     * @param normals: the vector of normals
     * @param v_index: the vector of vertex n_index
     * @param n_index: the vector of n_index
     * */
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(dir, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }
    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertices.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
    }

    for (size_t i = 0; i < attrib.normals.size(); i += 3) {
        normals.emplace_back(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]);
    }

    for (const auto &shape: shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                n_index.push_back(idx.vertex_index);
                v_index.push_back(idx.normal_index);
            }
            index_offset += fv;
        }
    }
    std::cout << "loading finished!" << std::endl;
}

