set(CMAKE_FOLDER "perf")

add_executable(EARanges_counted_insertion_sort counted_insertion_sort.cpp)
target_link_libraries(EARanges_counted_insertion_sort EARanges::EARanges)

add_executable(EARanges_range_conversion range_conversion.cpp)
target_link_libraries(EARanges_range_conversion EARanges::EARanges benchmark::benchmark benchmark::benchmark_main)

add_executable(EARanges_sort_patterns sort_patterns.cpp)
target_link_libraries(EARanges_sort_patterns EARanges::EARanges)
