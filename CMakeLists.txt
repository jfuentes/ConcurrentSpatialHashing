cmake_minimum_required(VERSION 3.6)
project(ConcurrentSpatialHashing)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/SpatialHashTable.cpp src/SpatialHashTable.h src/SpatialHashing.cpp src/SpatialHashing.h src/AABB.cpp src/AABB.h)
add_library (SpatialHashing ${SOURCE_FILES})
add_executable(ConcurrentSpatialHashing main.cpp)
target_link_libraries(ConcurrentSpatialHashing SpatialHashing)



add_subdirectory (test)