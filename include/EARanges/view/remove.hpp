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

#ifndef EARANGES_VIEW_REMOVE_HPP
#define EARANGES_VIEW_REMOVE_HPP


#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/view/remove_if.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        namespace views
        {
            struct remove_base_fn
            {
            private:
                template<typename Value>
                struct pred_
                {
                    Value value_;
                    template(typename T)(
                        requires equality_comparable_with<T, Value const &>) bool
                    operator()(T && other) const
                    {
                        return static_cast<T &&>(other) == value_;
                    }
                };

            public:
                template(typename Rng, typename Value)(
                    requires move_constructible<Value> AND viewable_range<Rng> AND
                        input_range<Rng>
                            AND indirectly_comparable<iterator_t<Rng>, Value const *,
                                                      equal_to>) constexpr auto
                operator()(Rng && rng, Value value) const
                {
                    return remove_if(static_cast<Rng &&>(rng),
                                     pred_<Value>{eastl::move(value)});
                }

                template(typename Rng, typename Value, typename Proj)(
                    requires move_constructible<Value> AND viewable_range<Rng> AND
                        input_range<Rng>
                            AND indirectly_comparable<iterator_t<Rng>, Value const *,
                                                      equal_to, Proj>) constexpr auto
                operator()(Rng && rng, Value value, Proj proj) const
                {
                    return remove_if(static_cast<Rng &&>(rng),
                                     pred_<Value>{eastl::move(value)},
                                     eastl::move(proj));
                }
            };

            struct remove_bind_fn
            {
                template<typename Value>
                constexpr auto operator()(
                    Value value) const // TODO: underconstrained - Eric Niebler
                {
                    return make_view_closure(
                        bind_back(remove_base_fn{}, eastl::move(value)));
                }
                template(typename Value, typename Proj)(
                    requires(!range<Value>)) // TODO: underconstrained - Eric Niebler
                    constexpr auto
                    operator()(Value && value, Proj proj) const
                {
                    return make_view_closure(bind_back(remove_base_fn{},
                                                       static_cast<Value &&>(value),
                                                       eastl::move(proj)));
                }
            };

            struct EARANGES_EMPTY_BASES remove_fn
              : remove_base_fn
              , remove_bind_fn
            {
                using remove_base_fn::operator();
                using remove_bind_fn::operator();
            };

            /// \relates remove_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(remove_fn, remove)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_VIEW_REMOVE_HPP