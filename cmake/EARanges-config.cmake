if (TARGET EARanges::meta)
  return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/EARanges-targets.cmake")

add_library(EARanges::meta INTERFACE IMPORTED)
add_library(EARanges::concepts INTERFACE IMPORTED)
add_library(EARanges::EARanges INTERFACE IMPORTED)

# workaround for target_link_libraries on lower cmake versions (< 3.11)
# see https://cmake.org/cmake/help/latest/release/3.11.html#commands
if(CMAKE_VERSION VERSION_LESS 3.11)
set_target_properties(EARanges::meta PROPERTIES INTERFACE_LINK_LIBRARIES "EARanges-meta")
set_target_properties(EARanges::concepts PROPERTIES INTERFACE_LINK_LIBRARIES "EARanges-concepts")
set_target_properties(EARanges::EARanges PROPERTIES INTERFACE_LINK_LIBRARIES "EARanges")
else()                
target_link_libraries(EARanges::meta INTERFACE EARanges-meta)
target_link_libraries(EARanges::concepts INTERFACE EARanges-concepts)
target_link_libraries(EARanges::EARanges INTERFACE EARanges)
endif()
