# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in subdirectories)

# Compilation flags
include(CheckCXXCompilerFlag)
macro(EARanges_append_flag testname flag)
  # As -Wno-* flags do not lead to build failure when there are no other
  # diagnostics, we check positive option to determine their applicability.
  # Of course, we set the original flag that is requested in the parameters.
  string(REGEX REPLACE "^-Wno-" "-W" alt ${flag})
  check_cxx_compiler_flag(${alt} ${testname})
  if (${testname})
    add_compile_options(${flag})
  endif()
endmacro()

function(cxx_standard_normalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x1y")
    set( ${return_value} "14" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "x1z")
    set( ${return_value} "17" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "xlatest" OR "x${cxx_standard}" STREQUAL "x2a")
    set( ${return_value} "20" PARENT_SCOPE )
  else()
    set( ${return_value} "${cxx_standard}" PARENT_SCOPE )
  endif()
endfunction()

function(cxx_standard_denormalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x17")
    if (EARANGES_CXX_COMPILER_CLANGCL OR EARANGES_CXX_COMPILER_MSVC)
      set( ${return_value} 17 PARENT_SCOPE )
    else()
      set( ${return_value} 1z PARENT_SCOPE )
    endif()
  elseif("x${cxx_standard}" STREQUAL "x20")
    if (EARANGES_CXX_COMPILER_CLANGCL OR EARANGES_CXX_COMPILER_MSVC)
      set( ${return_value} latest PARENT_SCOPE )
    else()
      set( ${return_value} 2a PARENT_SCOPE )
    endif()
  else()
    set( ${return_value} ${cxx_standard} PARENT_SCOPE )
  endif()
endfunction()

if(CMAKE_CXX_STANDARD)
  if(NOT "x${EARANGES_CXX_STD}" STREQUAL "xdefault")
    # Normalize EARANGES_CXX_STD
    cxx_standard_normalize( ${EARANGES_CXX_STD} EARanges_cxx_std )
    if(NOT "x${EARanges_cxx_std}" STREQUAL "x${CMAKE_CXX_STANDARD}")
      message(FATAL_ERROR "[EARanges]: Cannot specify both CMAKE_CXX_STANDARD and EARANGES_CXX_STD, or they must match.")
    endif()
  else()
    cxx_standard_denormalize(${CMAKE_CXX_STANDARD} EARANGES_CXX_STD)
  endif()
elseif("x${EARANGES_CXX_STD}" STREQUAL "xdefault")
  if (EARANGES_CXX_COMPILER_CLANGCL OR EARANGES_CXX_COMPILER_MSVC)
    set(EARANGES_CXX_STD 17)
  else()
    set(EARANGES_CXX_STD 14)
  endif()
endif()

# All compilation flags
# Language flag: version of the C++ standard to use
message(STATUS "[EARanges]: C++ std=${EARANGES_CXX_STD}")
if (EARANGES_CXX_COMPILER_CLANGCL OR EARANGES_CXX_COMPILER_MSVC)
  EARanges_append_flag(EARANGES_HAS_CXXSTDCOLON "/std:c++${EARANGES_CXX_STD}")
  set(EARANGES_STD_FLAG "/std:c++${EARANGES_CXX_STD}")
  if (EARANGES_CXX_COMPILER_CLANGCL)
    # The MSVC STL before VS 2019v16.6 with Clang 10 requires -fms-compatibility in C++17 mode, and
    # doesn't support C++20 mode at all. Let's drop this flag until AppVeyor updates to VS2016v16.6.
    # EARanges_append_flag(EARANGES_HAS_FNO_MS_COMPATIBIILITY "-fno-ms-compatibility")
    EARanges_append_flag(EARANGES_HAS_FNO_DELAYED_TEMPLATE_PARSING "-fno-delayed-template-parsing")
  endif()
  # Enable "normal" warnings and make them errors:
  EARanges_append_flag(EARANGES_HAS_W3 /W3)
  EARanges_append_flag(EARANGES_HAS_WX /WX)
else()
  EARanges_append_flag(EARANGES_HAS_CXXSTD "-std=c++${EARANGES_CXX_STD}")
  set(EARANGES_STD_FLAG "-std=c++${EARANGES_CXX_STD}")
  # Enable "normal" warnings and make them errors:
  EARanges_append_flag(EARANGES_HAS_WALL -Wall)
  EARanges_append_flag(EARANGES_HAS_WEXTRA -Wextra)
  if (EARANGES_ENABLE_WERROR)
    #EARanges_append_flag(EARANGES_HAS_WERROR -Werror)
  endif()
endif()

if (EARANGES_ENV_LINUX AND EARANGES_CXX_COMPILER_CLANG)
  # On linux libc++ re-exports the system math headers. The ones from libstdc++
  # use the GCC __extern_always_inline intrinsic which is not supported by clang
  # versions 3.6, 3.7, 3.8, 3.9, 4.0, and current trunk 5.0 (as of 2017.04.13).
  #
  # This works around it by replacing __extern_always_inline with inline using a
  # macro:
  EARanges_append_flag(EARANGES_HAS_D__EXTERN_ALWAYS_INLINE -D__extern_always_inline=inline)
endif()

# Deep integration support
if (EARANGES_DEEP_STL_INTEGRATION)
  if (EARANGES_CXX_COMPILER_MSVC)
    add_compile_options(/I "${PROJECT_SOURCE_DIR}/include/EARanges/std")
    add_compile_options(/I "${PROJECT_SOURCE_DIR}/include")
  else()
    add_compile_options(-isystem "${PROJECT_SOURCE_DIR}/include/EARanges/std")
    add_compile_options(-I "${PROJECT_SOURCE_DIR}/include")
  endif()
  add_compile_options(-DEARANGES_DEEP_STL_INTEGRATION=1)
endif()

# Template diagnostic flags
EARanges_append_flag(EARANGES_HAS_FDIAGNOSTIC_SHOW_TEMPLATE_TREE -fdiagnostics-show-template-tree)
EARanges_append_flag(EARANGES_HAS_FTEMPLATE_BACKTRACE_LIMIT "-ftemplate-backtrace-limit=0")
EARanges_append_flag(EARANGES_HAS_FMACRO_BACKTRACE_LIMIT "-fmacro-backtrace-limit=1")

# Clang modules support
if (EARANGES_MODULES)
  EARanges_append_flag(EARANGES_HAS_MODULES -fmodules)
  EARanges_append_flag(EARANGES_HAS_MODULE_MAP_FILE "-fmodule-map-file=${PROJECT_SOURCE_DIR}/include/EARanges/module.modulemap")
  EARanges_append_flag(EARANGES_HAS_MODULE_CACHE_PATH "-fmodules-cache-path=${PROJECT_BINARY_DIR}/module.cache")
  if (EARANGES_LIBCXX_MODULE)
    EARanges_append_flag(EARANGES_HAS_LIBCXX_MODULE_MAP_FILE "-fmodule-map-file=${EARANGES_LIBCXX_MODULE}")
  endif()
  if (EARANGES_ENV_MACOSX)
    EARanges_append_flag(EARANGES_HAS_NO_IMPLICIT_MODULE_MAPS -fno-implicit-module-maps)
  endif()
  if (EARANGES_DEBUG_BUILD)
    EARanges_append_flag(EARANGES_HAS_GMODULES -gmodules)
  endif()
endif()

# Sanitizer support: detect incompatible sanitizer combinations
if (EARANGES_ASAN AND EARANGES_MSAN)
  message(FATAL_ERROR "[EARanges error]: AddressSanitizer and MemorySanitizer are both enabled at the same time!")
endif()

if (EARANGES_MSAN AND EARANGES_ENV_MACOSX)
  message(FATAL_ERROR "[EARanges error]: MemorySanitizer is not supported on MacOSX!")
endif()

# AddressSanitizer support
if (EARANGES_ASAN)
  # This policy enables passing the linker flags to the linker when trying to test the features, which is required to successfully link ASan binaries
  cmake_policy(SET CMP0056 NEW)
  set (ASAN_FLAGS "")
  if (EARANGES_ENV_MACOSX) # LeakSanitizer not supported on MacOSX
    set (ASAN_FLAGS "-fsanitize=address,signed-integer-overflow,shift,integer-divide-by-zero,implicit-signed-integer-truncation,implicit-integer-sign-change,undefined,nullability")
  else()
    if (EARANGES_CXX_COMPILER_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0")
      set (ASAN_FLAGS "-fsanitize=address")
    else()
      set (ASAN_FLAGS "-fsanitize=address,signed-integer-overflow,shift,integer-divide-by-zero,implicit-signed-integer-truncation,implicit-integer-sign-change,leak,nullability")
    endif()
  endif()
  EARanges_append_flag(EARANGES_HAS_ASAN "${ASAN_FLAGS}")
  if (EARANGES_HAS_ASAN) #ASAN flags must be passed to the linker:
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS}")
  endif()
  EARanges_append_flag(EARANGES_HAS_SANITIZE_NO_RECOVER "-fno-sanitize-recover=all")
  EARanges_append_flag(EARANGES_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
endif()

# MemorySanitizer support
if (EARANGES_MSAN)
  # This policy enables passing the linker flags to the linker when trying to compile the examples, which is required to successfully link MSan binaries
  cmake_policy(SET CMP0056 NEW)
  EARanges_append_flag(EARANGES_HAS_MSAN "-fsanitize=memory")
  EARanges_append_flag(EARANGES_HAS_MSAN_TRACK_ORIGINS -fsanitize-memory-track-origins)
  EARanges_append_flag(EARANGES_HAS_SANITIZE_RECOVER_ALL "-fno-sanitize-recover=all")
  EARanges_append_flag(EARANGES_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
endif()

# Build types:
if (EARANGES_DEBUG_BUILD AND EARANGES_RELEASE_BUILD)
  message(FATAL_ERROR "[EARanges error] Cannot simultaneously generate debug and release builds!")
endif()

if (EARANGES_DEBUG_BUILD)
  EARanges_append_flag(EARRANGES_HAS_NO_INLINE -fno-inline)
  EARanges_append_flag(EARRANGES_HAS_STACK_PROTECTOR_ALL -fstack-protector-all)
  EARanges_append_flag(EARRANGES_HAS_G3 -g3)
  # Clang can generate debug info tuned for LLDB or GDB
  if (EARANGES_CXX_COMPILER_CLANG)
    if (EARANGES_ENV_MACOSX)
      EARanges_append_flag(EARANGES_HAS_GLLDB -glldb)
    elseif(EARANGES_ENV_LINUX OR EARANGES_ENV_OPENBSD)
      EARanges_append_flag(EARANGES_HAS_GGDB -ggdb)
    endif()
  endif()
endif()

if (EARANGES_RELEASE_BUILD)
  if (NOT EARANGES_ASSERTIONS)
    EARanges_append_flag(EARANGES_HAS_DNDEBUG -DNDEBUG)
  endif()
  if (NOT EARANGES_ASAN AND NOT EARANGES_MSAN)
    # The quality of ASan and MSan error messages suffers if we disable the frame pointer, so leave it enabled when compiling with either of them:
    EARanges_append_flag(EARANGES_HAS_OMIT_FRAME_POINTER -fomit-frame-pointer)
  endif()

  EARanges_append_flag(EARANGES_HAS_OFAST -Ofast)
  if (NOT EARANGES_HAS_OFAST)
    EARanges_append_flag(EARANGES_HAS_O2 -O2)
  endif()
  EARanges_append_flag(EARANGES_HAS_STRICT_ALIASING -fstrict-aliasing)
  EARanges_append_flag(EARANGES_HAS_STRICT_VTABLE_POINTERS -fstrict-vtable-pointers)
  EARanges_append_flag(EARANGES_HAS_FAST_MATH -ffast-math)
  EARanges_append_flag(EARANGES_HAS_VECTORIZE -fvectorize)

  if (NOT EARANGES_ENV_MACOSX)
    # Sized deallocation is not available in MacOSX:
    EARanges_append_flag(EARANGES_HAS_SIZED_DEALLOCATION -fsized-deallocation)
  endif()

  if (EARANGES_LLVM_POLLY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -polly -mllvm -polly-vectorizer=stripmine")
  endif()

  if (EARANGES_CXX_COMPILER_CLANG AND (NOT (EARANGES_INLINE_THRESHOLD EQUAL -1)))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -inline-threshold=${EARANGES_INLINE_THRESHOLD}")
  endif()
endif()

if (EARANGES_NATIVE)
  EARanges_append_flag(EARANGES_HAS_MARCH_NATIVE "-march=native")
  EARanges_append_flag(EARANGES_HAS_MTUNE_NATIVE "-mtune=native")
endif()

include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS ${EARANGES_STD_FLAG})
# Probe for library and compiler support for aligned new
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cmake/aligned_new_probe.cpp" EARANGE_PROBE_CODE)
check_cxx_source_compiles("${EARANGES_PROBE_CODE}" EARANGE_ALIGNED_NEW_PROBE)
unset(EARANGES_PROBE_CODE)
unset(CMAKE_REQUIRED_FLAGS)
if (NOT EARANGE_ALIGNED_NEW_PROBE)
  add_compile_options("-DEARANGES_CXX_ALIGNED_NEW=0")
endif()

# Probe for coroutine TS support
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cmake/coro_test_code.cpp" EARANGE_PROBE_CODE)
if(EARANGES_CXX_COMPILER_MSVC)
  set(CMAKE_REQUIRED_FLAGS "/await")
  check_cxx_source_compiles("${EARANGE_PROBE_CODE}" EARANGES_HAS_AWAIT)
  if(EARANGES_HAS_AWAIT)
    set(EARANGE_COROUTINE_FLAGS "/await")
  endif()
elseif(EARANGES_CXX_COMPILER_CLANG)
  set(CMAKE_REQUIRED_FLAGS "-fcoroutines-ts ${EARANGES_STD_FLAG}")
  check_cxx_source_compiles("${EARANGE_PROBE_CODE}" EARANGES_HAS_FCOROUTINES_TS)
  if(EARANGES_HAS_FCOROUTINES_TS)
    set(EARANGE_COROUTINE_FLAGS "-fcoroutines-ts")
  endif()
endif()
unset(CMAKE_REQUIRED_FLAGS)
unset(EARANGE_PROBE_CODE)
if (EARANGES_COROUTINE_FLAGS)
  add_compile_options(${EARANGES_COROUTINE_FLAGS})
endif()

# Test for concepts support
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cmake/concepts_test_code.cpp" EARANGES_PROBE_CODE)
if(EARANGES_CXX_COMPILER_GCC OR EARANGES_CXX_COMPILER_CLANG)
  set(CMAKE_REQUIRED_FLAGS "-fconcepts ${EARANGES_STD_FLAG}")
  check_cxx_source_compiles("${EARANGES_PROBE_CODE}" EARANGES_HAS_FCONCEPTS)
  if(EARANGES_HAS_FCONCEPTS)
    set(EARANGES_CONCEPTS_FLAGS "-fconcepts")
  endif()
endif()
unset(CMAKE_REQUIRED_FLAGS)
unset(EARANGES_PROBE_CODE)
if (EARANGES_CONCEPTS_FLAGS AND EARANGES_PREFER_REAL_CONCEPTS)
  add_compile_options(${EARANGES_CONCEPTS_FLAGS})
endif()

if (EARANGES_VERBOSE_BUILD)
  get_directory_property(EARANGES_COMPILE_OPTIONS COMPILE_OPTIONS)
  message(STATUS "[EARanges]: C++ flags: ${CMAKE_CXX_FLAGS}")
  message(STATUS "[EARanges]: C++ debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
  message(STATUS "[EARanges]: C++ Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
  message(STATUS "[EARanges]: C++ Compile Flags: ${CMAKE_CXX_COMPILE_FLAGS}")
  message(STATUS "[EARanges]: Compile options: ${EARANGES_COMPILE_OPTIONS}")
  message(STATUS "[EARanges]: C Flags: ${CMAKE_C_FLAGS}")
  message(STATUS "[EARanges]: C Compile Flags: ${CMAKE_C_COMPILE_FLAGS}")
  message(STATUS "[EARanges]: EXE Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
  message(STATUS "[EARanges]: C++ Linker flags: ${CMAKE_CXX_LINK_FLAGS}")
  message(STATUS "[EARanges]: MODULE Linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
  get_directory_property(CMakeCompDirDefs COMPILE_DEFINITIONS)
  message(STATUS "[EARanges]: Compile Definitions: ${CmakeCompDirDefs}")
endif()
