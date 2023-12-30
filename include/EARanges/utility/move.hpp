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

#ifndef EARANGES_UTILITY_MOVE_HPP
#define EARANGES_UTILITY_MOVE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace aux
    {
        /// \ingroup group-utility
        struct move_fn : move_tag
        {
            template<typename T>
            constexpr meta::_t<eastl::remove_reference<T>> && operator()(T && t) const //
                noexcept
            {
                return static_cast<meta::_t<eastl::remove_reference<T>> &&>(t);
            }

            /// \ingroup group-utility
            /// \sa `move_fn`
            template<typename T>
            friend constexpr decltype(auto) operator|(T && t, move_fn move) noexcept
            {
                return move(t);
            }
        };

        /// \ingroup group-utility
        /// \sa `move_fn`
        EARANGES_INLINE_VARIABLE(move_fn, move)

        /// \ingroup group-utility
        /// \sa `move_fn`
        template<typename R>
        using move_t = meta::if_c<eastl::is_reference<R>::value, meta::_t<eastl::remove_reference<R>> &&, detail::decay_t<R>>;
    } // namespace aux
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif