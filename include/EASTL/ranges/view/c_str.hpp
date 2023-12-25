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
#ifndef RANGES_V3_VIEW_C_STR_HPP
#define RANGES_V3_VIEW_C_STR_HPP

//#include <EASTL/type_traits.h>
#include "EASTL/type_traits.h"

#include "../range_fwd.hpp"

#include "../iterator/unreachable_sentinel.hpp"
#include "../utility/static_const.hpp"
#include "../view/delimit.hpp"
#include "../view/subrange.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T>
        struct is_char_type_ : eastl::false_type
        {};

        template<>
        struct is_char_type_<char> : eastl::true_type
        {};

        template<>
        struct is_char_type_<wchar_t> : eastl::true_type
        {};

        template<>
        struct is_char_type_<char16_t> : eastl::true_type
        {};

        template<>
        struct is_char_type_<char32_t> : eastl::true_type
        {};

        template<typename T>
        using is_char_type = is_char_type_<meta::_t<eastl::remove_cv<T>>>;
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    namespace views
    {
        /// View a `\0`-terminated C string (e.g. from a const char*) as a range.
        struct c_str_fn
        {
            // Fixed-length
            template(typename Char, std::size_t N)(requires detail::is_char_type<Char>::value) //
            ranges::subrange<Char *> operator()(Char (&sz)[N]) const
            {
                return {&sz[0], &sz[N - 1]};
            }

            // Null-terminated
            template(typename Char)(requires detail::is_char_type<Char>::value) //
            ranges::delimit_view<ranges::subrange<Char *, ranges::unreachable_sentinel_t>, meta::_t<eastl::remove_cv<Char>>> //
            operator()(Char * sz) const volatile
            {
                using ch_t = meta::_t<eastl::remove_cv<Char>>;
                return ranges::views::delimit(sz, ch_t(0));
            }
        };

        /// \relates c_str_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(c_str_fn, c_str)
    } // namespace views
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif