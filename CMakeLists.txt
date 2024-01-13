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

set(VCPKG_LIBRARY_LINKAGE static)
#include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
include("vcpkg/scripts/buildsystems/vcpkg.cmake")

project(EARanges CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(EASTL CONFIG REQUIRED)

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

# Add a custom target to move the "extra" folder to vcpkg output include folder
add_custom_target(move_extra_eastl_folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/extra
            ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/EASTL
    COMMENT "Moving 'extra' folder to vcpkg output include folder"
    VERBATIM
)

function(EARanges_add_test TESTNAME EXENAME FIRSTSOURCE)
  add_executable(EARanges.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(EARanges.${EXENAME} EARanges)
  add_test(EARanges.${TESTNAME} EARanges.${EXENAME})
  # Add the custom target as a dependency for your main target
add_custom_target(EARanges.${TESTNAME} ALL
    DEPENDS move_extra_eastl_contents
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/extra
)
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
  message(STATUS "Building perfs!")
  include(gtest)
  include(gbenchmark)
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

# If the new version.hpp is materially different from the one in the source directory, update it, commit, and tag.
#TODO: or maybe not, who cares.
#if(NOT EARANGES_NEW_VERSION_HPP STREQUAL EARANGES_OLD_VERSION_HPP)
#  # Check that doc/release_notes.md and Version.cmake are the only changed file:
#  execute_process(
#    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" status --porcelain -uno
#    OUTPUT_VARIABLE RANGE_V3_GIT_STATUS
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#  )
#  string(REPLACE "\n" ";"  RANGE_V3_GIT_STATUS ${RANGE_V3_GIT_STATUS})
#  if (NOT "x${RANGE_V3_GIT_STATUS}" STREQUAL "x M Version.cmake; M doc/release_notes.md")
#    message(FATAL_ERROR "Cannot update version.hpp: range-v3 source directory has a dirty status")
#  endif()
#  file(
#    COPY ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp
#    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3
#  )
#  execute_process(
#    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" add -u
#  )
#  execute_process(
#    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" commit -m "${RANGE_V3_VERSION}"
#  )
#  execute_process(
#    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" tag -f -a "${RANGE_V3_VERSION}" -m "${RANGE_V3_VERSION}"
#  )
#  find_program(CONAN_EXECUTABLE NAMES conan conan.exe)
#  if (NOT "x${CONAN_EXECUTABLE}" STREQUAL "xCONAN_EXECUTABLE-NOTFOUND")
#    message(STATUS "Exporting conanfile for new version")
#    execute_process(
#      COMMAND ${CONAN_EXECUTABLE} create . range-v3/${RANGE_V3_VERSION}@ericniebler/stable
#      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#    )
#  endif()
#  message(STATUS "Version updated to ${RANGE_V3_VERSION}. Don't forget to:")
#  message(STATUS "  git push origin <feature-branch>")
#  message(STATUS "and (after that is merged to master) then:")
#  message(STATUS "  conan remote add range-v3 https://api.bintray.com/conan/range-v3/range-v3")
#  message(STATUS "  conan create ${CMAKE_CURRENT_SOURCE_DIR} range-v3/${RANGE_V3_VERSION}@ericniebler/stable")
#  message(STATUS "  conan upload --all range-v3/${RANGE_V3_VERSION}@ericniebler/stable -r=range-v3")
#endif()


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
