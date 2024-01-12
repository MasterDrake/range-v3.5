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
#ifndef EARANGES_UTILITY_ADDRESSOF_HPP
#define EARANGES_UTILITY_ADDRESSOF_HPP

#include <EASTL/memory.h>

#include <EASTL/type_traits.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/detail/config.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        using eastl::addressof;
    } // namespace detail
    /// \endcond
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif