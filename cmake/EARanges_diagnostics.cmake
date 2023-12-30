# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in subdirectories)

# Enable all warnings:
EARanges_append_flag(EARANGES_HAS_WEVERYTHING -Weverything)
EARanges_append_flag(EARANGES_HAS_PEDANTIC -pedantic)
EARanges_append_flag(EARANGES_HAS_PEDANTIC_ERRORS -pedantic-errors)

# Selectively disable those warnings that are not worth it:
EARanges_append_flag(EARANGES_HAS_WNO_CXX98_COMPAT -Wno-c++98-compat)
EARanges_append_flag(EARANGES_HAS_WNO_CXX98_COMPAT_PEDANTIC -Wno-c++98-compat-pedantic)
EARanges_append_flag(EARANGES_HAS_WNO_WEAK_VTABLES -Wno-weak-vtables)
EARanges_append_flag(EARANGES_HAS_WNO_PADDED -Wno-padded)
EARanges_append_flag(EARANGES_HAS_WNO_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations)
EARanges_append_flag(EARANGES_HAS_WNO_DOCUMENTATION -Wno-documentation)
EARanges_append_flag(EARANGES_HAS_WNO_DOCUMENTATION_UNKNOWN_COMMAND -Wno-documentation-unknown-command)
EARanges_append_flag(EARANGES_HAS_WNO_OLD_STYLE_CAST -Wno-old-style-cast)

if (EARANGES_ENV_MACOSX)
  EARanges_append_flag(EARANGES_HAS_WNO_GLOBAL_CONSTRUCTORS -Wno-global-constructors)
  EARanges_append_flag(EARANGES_HAS_WNO_EXIT_TIME_DESTRUCTORS -Wno-exit-time-destructors)
endif()

if (EARANGES_CXX_COMPILER_CLANG OR EARANGES_CXX_COMPILER_CLANGCL)
  EARanges_append_flag(EARANGES_HAS_WNO_MISSING_PROTOTYPES -Wno-missing-prototypes)
endif()

if (EARANGES_CXX_COMPILER_GCC)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
    EARanges_append_flag(EARANGES_HAS_WNO_STRICT_OVERFLOW -Wno-strict-overflow)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
      EARanges_append_flag(EARANGES_HAS_WNO_MISSING_FIELD_INITIALIZERS -Wno-missing-field-initializers)
    endif()
  elseif ((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0") OR (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "7.0"))
    EARanges_append_flag(EARANGES_HAS_WNO_NOEXCEPT_TYPE -Wno-noexcept-type)
  endif()
endif()

if (EAREANGES_VERBOSE_BUILD)
  message(STATUS "[EARanges]: test C++ flags: ${CMAKE_CXX_FLAGS}")
endif()
