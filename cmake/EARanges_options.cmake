# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# CMake options

include(CMakeDependentOption)

set(EARANGES_CXX_STD default CACHE STRING "C++ standard version.")
option(EARANGES_BUILD_CALENDAR_EXAMPLE "Builds the calendar example." ON)
option(EARANGES_ASAN "Run the tests using AddressSanitizer." OFF)
option(EARANGES_MSAN "Run the tests using MemorySanitizer." OFF)
option(EARANGES_ASSERTIONS "Enable assertions." ON)
option(EARANGES_DEBUG_INFO "Include debug information in the binaries." ON)
option(EARANGES_MODULES "Enables use of Clang modules (experimental)." OFF)
option(EARANGES_NATIVE "Enables -march/-mtune=native." ON)
option(EARANGES_VERBOSE_BUILD "Enables debug output from CMake." OFF)
option(EARANGES_LLVM_POLLY "Enables LLVM Polly." OFF)
option(EARANGES_ENABLE_WERROR
  "Enables -Werror. Only effective if compiler is not clang-cl or MSVC. OFF by default"
  OFF)
option(EARANGES_PREFER_REAL_CONCEPTS
  "Use real concepts instead of emulation if the compiler supports it"
  ON)
option(EARANGES_DEEP_STL_INTEGRATION
  "Hijacks the primary std::iterator_traits template to emulate the C++20 std::ranges:: behavior."
  OFF)
option(EARANGES_HEADER_CHECKS
  "Build the EARanges header checks and integrate with ctest"
  OFF)
set(EARANGES_INLINE_THRESHOLD -1 CACHE STRING "Force a specific inlining threshold.")

# Enable verbose configure when passing -Wdev to CMake
if (DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS AND NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(EARANGES_VERBOSE_BUILD ON)
endif()

if (EARANGES_VERBOSE_BUILD)
  message(STATUS "[EARanges]: verbose build enabled.")
endif()

CMAKE_DEPENDENT_OPTION(EARANGES_INSTALL
  "Generate an install target for EARanges"
  ON "${is_standalone}" OFF)

CMAKE_DEPENDENT_OPTION(EARANGES_TESTS
  "Build the EARANGES tests and integrate with ctest"
  ON "${is_standalone}" OFF)

CMAKE_DEPENDENT_OPTION(EARANGES_EXAMPLES
  "Build the EARanges examples and integrate with ctest"
  ON "${is_standalone}" OFF)

option(EARANGES_PERF
  "Build the EARanges performance benchmarks"
  ON)

CMAKE_DEPENDENT_OPTION(EARANGES_DOCS
  "Build the EARanges documentation"
  ON "${is_standalone}" OFF)

#mark_as_advanced(EARANGES_PERF)
