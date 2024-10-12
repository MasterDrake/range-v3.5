# Copyright Eric Niebler 2014
# Copyright Gonzalo Brito Gadeschi 2014, 2017
# Copyright Louis Dionne 2015
# Copyright Casey Carter 2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.6)
get_directory_property(is_subproject PARENT_DIRECTORY)

if(NOT is_subproject)
    set(is_standalone YES)
else()
    set(is_standalone NO)
endif()

if (WIN32)
	set(VCPKG_TARGET_TRIPLET x64-windows-static)
	include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
elseif (UNIX)
	set(VCPKG_TARGET_TRIPLET x64-linux)
 	include("/home/ilies/GameDev/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

set(VCPKG_LIBRARY_LINKAGE static)

project(EARanges CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set(EASTL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/first-party)
#add_subdirectory(first-party/EASTL)

find_package(EASTL CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

include(EARanges_options)
include(EARanges_env)
include(EARanges_flags)

add_library(EARanges-meta INTERFACE)
add_library(EARanges::meta ALIAS EARanges-meta)
target_include_directories(EARanges-meta INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(EARanges-meta SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_compile_options(EARanges-meta INTERFACE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
                                               $<$<COMPILE_LANG_AND_ID:CUDA,MSVC>:-Xcompiler=/permissive->)

target_link_libraries(EARanges-meta INTERFACE EASTL)

add_library(EARanges-concepts INTERFACE)
add_library(EARanges::concepts ALIAS EARanges-concepts)
target_include_directories(EARanges-concepts INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(EARanges-concepts SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_compile_options(EARanges-concepts INTERFACE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
                                                   $<$<COMPILE_LANG_AND_ID:CUDA,MSVC>:-Xcompiler=/permissive->)
target_link_libraries(EARanges-concepts INTERFACE EARanges::meta)

target_link_libraries(EARanges-concepts INTERFACE EASTL)

add_library(EARanges INTERFACE)
add_library(EARanges::EARanges ALIAS EARanges)
target_include_directories(EARanges INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(EARanges SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_compile_options(EARanges INTERFACE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
                                          $<$<COMPILE_LANG_AND_ID:CUDA,MSVC>:-Xcompiler=/permissive->)
target_link_libraries(EARanges INTERFACE EARanges::concepts EARanges::meta)

target_link_libraries(EARanges INTERFACE EASTL)

function(EARanges_add_test TESTNAME EXENAME FIRSTSOURCE)
  add_executable(EARanges.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(EARanges.${EXENAME} EARanges)
  add_test(EARanges.${TESTNAME} EARanges.${EXENAME})
endfunction(EARanges_add_test)

if(EARANGES_DOCS)
  add_subdirectory(doc)
endif()

if(EARANGES_TESTS)
  include(CTest)
  add_subdirectory(test)
endif()

if(EARANGES_EXAMPLES)
  add_subdirectory(example)
endif()

if(EARANGES_PERF)
  message(STATUS "Building perf tests!")
  add_subdirectory(perf)
endif()

# Add header files as sources to fix MSVS 2017 not finding source during debugging
file(GLOB_RECURSE EARANGES_PUBLIC_HEADERS_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
add_custom_target(EARanges.headers SOURCES ${EARANGES_PUBLIC_HEADERS_ABSOLUTE})
set_target_properties(EARanges.headers PROPERTIES FOLDER "header")

# Test all headers
if(EARANGES_HEADER_CHECKS)
  include(TestHeaders)

  file(GLOB_RECURSE EARANGES_PUBLIC_HEADERS
                    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
  # These headers are not meant to be included directly:
  list(REMOVE_ITEM EARANGES_PUBLIC_HEADERS std/detail/associated_types.hpp)
  list(REMOVE_ITEM EARANGES_PUBLIC_HEADERS detail/epilogue.hpp)
  # Deprecated headers
  if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    foreach(header ${EARANGES_PUBLIC_HEADERS})
      file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/${header}" is_deprecated
           LIMIT_COUNT 1
           REGEX ".*EARANGES_DEPRECATED_HEADER.*")
      if(is_deprecated)
        list(APPEND EARANGES_DEPRECATED_PUBLIC_HEADERS "${header}")
      endif()
    endforeach()
  endif()
  
  foreach(deprecated_header ${EARANGES_DEPRECATED_PUBLIC_HEADERS})
	message(STATUS "Deprecated header ${deprecated_header}")
  endforeach()
	
  if(NOT EARANGES_COROUTINE_FLAGS)
      # Can't test this header if we can't even compile it
      list(APPEND EARANGES_DEPRECATED_PUBLIC_HEADERS experimental/utility/generator.hpp)
  endif()

  add_header_test(test.EARanges.headers
    EXCLUDE ${EARANGES_DEPRECATED_PUBLIC_HEADERS}
    HEADERS ${EARANGES_PUBLIC_HEADERS})
  target_link_libraries(test.EARanges.headers PRIVATE EARanges)
endif()

# Grab the EARanges version numbers:
include(${CMAKE_CURRENT_SOURCE_DIR}/Version.cmake)
set(EARANGES_VERSION ${EARANGES_MAJOR}.${EARANGES_MINOR}.${EARANGES_PATCHLEVEL})

# Try to build a new version.hpp
configure_file(version.hpp.in include/EARanges/version.hpp @ONLY)
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/include/EARanges/version.hpp EARANGES_OLD_VERSION_HPP)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/EARanges/version.hpp EARANGES_NEW_VERSION_HPP)

if (EARANGES_INSTALL)
  include(CMakePackageConfigHelpers)

  # write_basic_package_version_file(...) gained ARCH_INDEPENDENT in CMake 3.14.
  # For CMake 3.6, this workaround makes the version file ARCH_INDEPENDENT
  # by making CMAKE_SIZEOF_VOID_P empty.
  set(OLD_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
  set(CMAKE_SIZEOF_VOID_P "")
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EARanges-config-version.cmake
    VERSION ${EARANGES_VERSION}
    COMPATIBILITY ExactVersion
  )
  set(CMAKE_SIZEOF_VOID_P ${OLD_CMAKE_SIZEOF_VOID_P})

  include(GNUInstallDirs)
  install(TARGETS EARanges-concepts EARanges-meta EARanges EXPORT EARanges-targets DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(EXPORT EARanges-targets FILE EARanges-targets.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EARanges)
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EARanges-config-version.cmake
    cmake/EARanges-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EARanges)
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*")

  export(EXPORT EARanges-targets FILE EARanges-config.cmake)
endif()


#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/extra/ DESTINATION ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/EASTL)
