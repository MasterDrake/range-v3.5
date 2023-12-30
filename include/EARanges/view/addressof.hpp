/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_ADDRESSOF_HPP
#define EARANGES_VIEW_ADDRESSOF_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/utility/addressof.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct addressof_fn
        {
        private:
            struct take_address
            {
                template<typename V>
                constexpr V * operator()(V & value) const noexcept
                {
                    return detail::addressof(value);
                }
            };

        public:
            template(typename Rng)(requires viewable_range<Rng> AND input_range<Rng> AND eastl::is_lvalue_reference<range_reference_t<Rng>>::value) //
            constexpr auto CPP_auto_fun(operator())(Rng && rng)(const) //
            (
                return transform(all(static_cast<Rng &&>(rng)), take_address{}) //
            )
        };

        /// \relates addressof_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(view_closure<addressof_fn>, addressof)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_VIEW_ADDRESSOF_HPP