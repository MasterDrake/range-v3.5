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
#ifndef EARANGES_ITERATOR_UNREACHABLE_SENTINEL_HPP
#define EARANGES_ITERATOR_UNREACHABLE_SENTINEL_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{
    struct unreachable_sentinel_t
    {
        template<typename I>
        friend constexpr auto operator==(I const &, unreachable_sentinel_t) noexcept -> CPP_broken_friend_ret(bool)(requires weakly_incrementable<I>)
        {
            return false;
        }
        template<typename I>
        friend constexpr auto operator==(unreachable_sentinel_t, I const &) noexcept -> CPP_broken_friend_ret(bool)(requires weakly_incrementable<I>)
        {
            return false;
        }
        template<typename I>
        friend constexpr auto operator!=(I const &, unreachable_sentinel_t) noexcept -> CPP_broken_friend_ret(bool)(requires weakly_incrementable<I>)
        {
            return true;
        }
        template<typename I>
        friend constexpr auto operator!=(unreachable_sentinel_t, I const &) noexcept -> CPP_broken_friend_ret(bool)(requires weakly_incrementable<I>)
        {
            return true;
        }
    };

    EARANGES_INLINE_VARIABLE(unreachable_sentinel_t, unreachable)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ITERATOR_UNREACHABLE_SENTINEL_HPP