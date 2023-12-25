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
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

//#include <EASTL/ttype_traits.h>
#include "EASTL/type_traits.h"

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"

#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/primitives.hpp"
#include "../utility/static_const.hpp"
#include "../view/ref.hpp"
#include "../view/subrange.hpp"
#include "../view/view.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct all_fn
        {
        private:
            /// If it's a view already, pass it though.
            template<typename T>
            static constexpr auto from_range_(T && t, eastl::true_type, detail::ignore_t, detail::ignore_t)
            {
                return static_cast<T &&>(t);
            }

            /// If it is container-like, turn it into a view, being careful
            /// to preserve the Sized-ness of the range.
            template<typename T>
            static constexpr auto from_range_(T && t, eastl::false_type, eastl::true_type, detail::ignore_t)
            {
                return ranges::views::ref(t);
            }

            /// Not a view and not an lvalue? If it's a borrowed_range, then
            /// return a subrange holding the range's begin/end.
            template<typename T>
            static constexpr auto from_range_(T && t, eastl::false_type, eastl::false_type, eastl::true_type)
            {
                return make_subrange(static_cast<T &&>(t));
            }

        public:
            template(typename T)(requires range<T &> AND viewable_range<T>)
            constexpr auto operator()(T && t) const
            {
                return all_fn::from_range_(static_cast<T &&>(t),
                                           meta::bool_<view_<uncvref_t<T>>>{},
                                           eastl::is_lvalue_reference<T>{},
                                           meta::bool_<borrowed_range<T>>{});
            }

        };

        /// \relates all_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view_closure<all_fn>, all)

        template<typename Rng>
        using all_t = decltype(all(eastl::declval<Rng>()));
    } // namespace views

    template<typename Rng>
    struct identity_adaptor : Rng
    {
        CPP_assert(view_<Rng>);

        identity_adaptor() = default;
        constexpr explicit identity_adaptor(Rng const & rng)
          : Rng(rng)
        {}
        constexpr explicit identity_adaptor(Rng && rng)
          : Rng(detail::move(rng))
        {}
    };

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif