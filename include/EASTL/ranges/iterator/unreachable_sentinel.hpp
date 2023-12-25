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
#ifndef RANGES_V3_ITERATOR_UNREACHABLE_SENTINEL_HPP
#define RANGES_V3_ITERATOR_UNREACHABLE_SENTINEL_HPP

#include "../range_fwd.hpp"

#include "../iterator/concepts.hpp"

#include "../detail/prologue.hpp"

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

    RANGES_INLINE_VARIABLE(unreachable_sentinel_t, unreachable)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_ITERATOR_UNREACHABLE_SENTINEL_HPP