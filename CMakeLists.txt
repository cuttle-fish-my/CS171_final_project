cmake_minimum_required(VERSION 3.23)
project(CS171_final_project)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
file(GLOB SRC_FILES src/*.cpp)

add_subdirectory(libs)

add_executable(CS171_final_project ${SRC_FILES})
target_include_directories(CS171_final_project PRIVATE include)

target_link_libraries(CS171_final_project OpenVDB::openvdb stb tinyobjloader)
