#ifndef CS171_FINAL_PROJECT_INTERACTION_H
#define CS171_FINAL_PROJECT_INTERACTION_H

#include "common.h"

struct Interaction {
    Vec3f pos{0,0,0};
    float dist{RAY_DEFAULT_MAX};
    Vec3f normal{0,0,0};
};

#endif //CS171_FINAL_PROJECT_INTERACTION_H
