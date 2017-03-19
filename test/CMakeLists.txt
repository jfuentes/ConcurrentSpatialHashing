

add_executable (Test catch.hh SpatialHashTable.cpp)
target_link_libraries(Test SpatialHashing)

add_test (NAME MyTest COMMAND Test)