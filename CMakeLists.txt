cmake_minimum_required(VERSION 3.23)
project(CS171_final_project)

set(CMAKE_CXX_STANDARD 14)
list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
add_executable(CS171_final_project test.cpp)
target_link_libraries(CS171_final_project OpenVDB::openvdb)
