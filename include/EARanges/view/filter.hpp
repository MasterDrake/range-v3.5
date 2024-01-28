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

#ifndef EARANGES_VIEW_FILTER_HPP
#define EARANGES_VIEW_FILTER_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/compose.hpp>
#include <EARanges/functional/not_fn.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/remove_if.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct filter_view : remove_if_view<Rng, logical_negate<Pred>>
        {
            filter_view() = default;
            constexpr filter_view(Rng rng, Pred pred)
              : filter_view::remove_if_view{eastl::move(rng), not_fn(eastl::move(pred))}
            {}
        };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename Rng, typename Pred)(
            requires input_range<Rng> AND indirect_unary_predicate<Pred, iterator_t<Rng>>
                AND view_<Rng>
                    AND eastl::is_object<Pred>::value) //
            filter_view(Rng &&, Pred) -> filter_view<views::all_t<Rng>, Pred>;
#endif

        namespace views
        {
            struct filter_fn;

            /// Given a source range and a unary predicate,
            /// present a view of the elements that satisfy the predicate.
            struct cpp20_filter_base_fn
            {
                template(typename Rng, typename Pred)(
                    requires viewable_range<Rng> AND input_range<Rng> AND
                        indirect_unary_predicate<
                            Pred, iterator_t<Rng>>) constexpr filter_view<all_t<Rng>,
                                                                          Pred>
                operator()(Rng && rng, Pred pred) //
                    const
                {
                    return filter_view<all_t<Rng>, Pred>{all(static_cast<Rng &&>(rng)),
                                                         eastl::move(pred)};
                }
            };

            struct cpp20_filter_fn : cpp20_filter_base_fn
            {
                using cpp20_filter_base_fn::operator();

                template<typename Pred>
                constexpr auto operator()(Pred pred) const
                {
                    return make_view_closure(
                        bind_back(cpp20_filter_base_fn{}, eastl::move(pred)));
                }
            };

            /// Given a source range, unary predicate, and optional projection,
            /// present a view of the elements that satisfy the predicate.
            struct filter_base_fn : cpp20_filter_base_fn
            {
                using cpp20_filter_base_fn::operator();

                template(typename Rng, typename Pred, typename Proj)(
                    requires viewable_range<Rng> AND input_range<Rng> AND
                        indirect_unary_predicate<
                            Pred,
                            projected<iterator_t<Rng>,
                                      Proj>>) constexpr filter_view<all_t<Rng>,
                                                                    composed<Pred,
                                                                             Proj>> //
                operator()(Rng && rng, Pred pred, Proj proj) const
                {
                    return filter_view<all_t<Rng>, composed<Pred, Proj>>{
                        all(static_cast<Rng &&>(rng)),
                        compose(eastl::move(pred), eastl::move(proj))};
                }
            };

            /// # ranges::views::filter
            /// The filter view takes in a predicate function `T -> bool` and converts an
            /// input range of `T` into an output range of `T` by keeping all elements for
            /// which the predicate returns true.
            ///
            /// ## Example
            /// \snippet example/view/filter.cpp filter example
            ///
            /// ### Output
            /// \include example/view/filter_golden.txt
            ///
            /// ## Syntax
            /// ```cpp
            /// auto output_range = input_range | ranges::views::filter(filter_func);
            /// ```
            ///
            /// ## Parameters
            /// <pre><b>filter_func</b></pre>
            ///   - Called once for each element of the input range
            ///   - Returns true for elements that should present in the output range
            ///
            /// <pre><b>input_range</b></pre>
            ///   - The range of elements to filter
            ///   - Reference type: `T`
            ///
            /// <pre><b>output_range</b></pre>
            ///   - The range of filtered values
            ///     - Is either a `forward_range` or the concept satisfied by the input
            ///     - Is a `common_range` if the input is a `common_range`
            ///     - Is not a `sized_range` or `borrowed_range`
            ///   - Reference type: `T`
            ///
            struct filter_fn : filter_base_fn
            {
                using filter_base_fn::operator();

                template<typename Pred>
                constexpr auto operator()(Pred pred) const
                {
                    return make_view_closure(
                        bind_back(filter_base_fn{}, eastl::move(pred)));
                }

                template(typename Pred,
                         typename Proj)(requires(!range<Pred>)) constexpr auto
                operator()(Pred pred, Proj proj) const
                {
                    return make_view_closure(bind_back(
                        filter_base_fn{}, eastl::move(pred), eastl::move(proj)));
                }
            };

            /// \relates filter_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(filter_fn, filter)
        } // namespace views

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::filter_view)

#endif
