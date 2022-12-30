#ifndef CS171_FINAL_PROJECT_UTILS_H
#define CS171_FINAL_PROJECT_UTILS_H

#include "common.h"

namespace util {
    static inline float clamp01(float v) {
        if (v > 1) v = 1;
        else if (v < 0) v = 0;
        return v;
    }

    static inline uint8_t gammaCorrection(float radiance) {
        return static_cast<uint8_t>(255.f * clamp01(powf(radiance, 1.f / 2.2f)));
    }

    static std::vector<floatGrid::Ptr> convertVectorGridsToScalarGrids(const std::vector<Vec3fGrid::Ptr> &vectorGrids) {
        std::vector<floatGrid::Ptr> scalarGrids;
        for (auto &grid: vectorGrids) {
            auto scalarGrid = floatGrid::create();
            scalarGrid->setTransform(grid->transform().copy());
            scalarGrid->topologyUnion(*grid);
            for (auto iter = grid->beginMeta(); iter != grid->endMeta(); ++iter) {
                scalarGrid->insertMeta(openvdb::Name(iter->first),*(iter->second));
            }
            auto accessor = scalarGrid->getAccessor();
            for (auto iter = grid->cbeginValueOn(); iter; ++iter) {
                accessor.setValue(iter.getCoord(), iter.getValue().length());
            }
            scalarGrids.push_back(scalarGrid);
        }
        return scalarGrids;
    }
}

#endif //CS171_FINAL_PROJECT_UTILS_H
