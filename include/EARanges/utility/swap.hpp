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
// The implementation of swap (see below) has been adapted from libc++
// (http://libcxx.llvm.org).

#ifndef EARANGES_UTILITY_SWAP_HPP
#define EARANGES_UTILITY_SWAP_HPP

#include <EARanges/concepts/swap.hpp>

#include <EARanges/range_fwd.hpp>
      
#include <EARanges/utility/static_const.hpp>
      
#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    template<typename T>
    using is_swappable = concepts::is_swappable<T>;

    template<typename T>
    using is_nothrow_swappable = concepts::is_nothrow_swappable<T>;

    template<typename T, typename U>
    using is_swappable_with = concepts::is_swappable_with<T, U>;

    template<typename T, typename U>
    using is_nothrow_swappable_with = concepts::is_nothrow_swappable_with<T, U>;

    using concepts::exchange;

    /// \ingroup group-utility
    /// \relates concepts::adl_swap_detail::swap_fn
    EARANGES_DEFINE_CPO(uncvref_t<decltype(concepts::swap)>, swap)
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif