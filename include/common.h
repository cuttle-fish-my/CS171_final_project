#ifndef CS171_FINAL_PROJECT_COMMON_H
#define CS171_FINAL_PROJECT_COMMON_H

#include <openvdb/openvdb.h>

using Vec2i = openvdb::Vec2i;

using Vec3i = openvdb::Vec3i;
using Vec3f = openvdb::Vec3f;
using Vec3d = openvdb::Vec3d;
#define RAY_DEFAULT_MIN 1e-5
#define RAY_DEFAULT_MAX 1e7
#define PI 3.141592653579f
#define INV_PI 0.31830988618379067154f
#endif //CS171_FINAL_PROJECT_COMMON_H
