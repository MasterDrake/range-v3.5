/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ITERATOR_DEFAULT_SENTINEL_HPP
#define EARANGES_ITERATOR_DEFAULT_SENTINEL_HPP

#include <EARanges/detail/config.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{
    struct default_sentinel_t
    {};

    // Default sentinel
    EARANGES_INLINE_VARIABLE(default_sentinel_t, default_sentinel)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif