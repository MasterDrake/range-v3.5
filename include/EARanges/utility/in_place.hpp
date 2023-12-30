/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_UTILITY_IN_PLACE_HPP
#define EARANGES_UTILITY_IN_PLACE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \ingroup group-utility
    struct in_place_t
    {};
    EARANGES_INLINE_VARIABLE(in_place_t, in_place)
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif