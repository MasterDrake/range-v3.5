set(CMAKE_FOLDER "${CMAKE_FOLDER}/range")

EARanges_add_test(test.range.access range.access access.cpp)
EARanges_add_test(test.range.conversion range.conversion conversion.cpp)
EARanges_add_test(test.range.index range.index index.cpp)
EARanges_add_test(test.range.operations range.operations operations.cpp)
