list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/)
find_package( TBB )
if ( NOT TBB_FOUND )
    message(STATUS "This project requires the Intel TBB library, and will not be compiled.")
    return()
endif()

include_directories(${TBB_INCLUDE_DIRS})
add_definitions(${TBB_DEFINITIONS})

add_executable (Test catch.hh SpatialHashTable.cpp)
target_link_libraries(Test SpatialHashing ${TBB_LIBRARIES})

add_test (NAME MyTest COMMAND Test)