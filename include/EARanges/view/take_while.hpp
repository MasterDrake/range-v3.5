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

#ifndef EARANGES_VIEW_TAKE_WHILE_HPP
#define EARANGES_VIEW_TAKE_WHILE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/compose.hpp>
#include <EARanges/functional/indirect.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adaptor.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Pred>
    struct iter_take_while_view
      : view_adaptor<iter_take_while_view<Rng, Pred>, Rng,
                     is_finite<Rng>::value ? finite : unknown>
    {
    private:
        friend range_access;
        EARANGES_NO_UNIQUE_ADDRESS semiregular_box_t<Pred> pred_;

        template<bool IsConst>
        struct sentinel_adaptor : adaptor_base
        {
        private:
            friend struct sentinel_adaptor<!IsConst>;
            using CRng = meta::const_if_c<IsConst, Rng>;
            EARANGES_NO_UNIQUE_ADDRESS semiregular_box_ref_or_val_t<Pred, IsConst> pred_;

        public:
            sentinel_adaptor() = default;
            sentinel_adaptor(semiregular_box_ref_or_val_t<Pred, IsConst> pred)
              : pred_(eastl::move(pred))
            {}
            template(bool Other)(
                requires IsConst AND CPP_NOT(Other)) //
                sentinel_adaptor(sentinel_adaptor<Other> that)
              : pred_(eastl::move(that.pred_))
            {}
            bool empty(iterator_t<CRng> const & it, sentinel_t<CRng> const & last) const
            {
                return it == last || !invoke(pred_, it);
            }
        };
        sentinel_adaptor<false> end_adaptor()
        {
            return {pred_};
        }
        template(bool Const = true)(
            requires Const AND range<meta::const_if_c<Const, Rng>> AND
                invocable<Pred const &, iterator_t<meta::const_if_c<Const, Rng>>>)
        sentinel_adaptor<Const> end_adaptor() const
        {
            return {pred_};
        }

    public:
        iter_take_while_view() = default;
        constexpr iter_take_while_view(Rng rng, Pred pred)
          : iter_take_while_view::view_adaptor{eastl::move(rng)}
          , pred_(eastl::move(pred))
        {}
    };

    template<typename Rng, typename Pred>
    struct take_while_view : iter_take_while_view<Rng, indirected<Pred>>
    {
        take_while_view() = default;
        constexpr take_while_view(Rng rng, Pred pred)
          : iter_take_while_view<Rng, indirected<Pred>>{eastl::move(rng), indirect(eastl::move(pred))}
        {}
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template(typename Rng, typename Fun)(
        requires copy_constructible<Fun>)
    take_while_view(Rng &&, Fun) -> take_while_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct iter_take_while_base_fn
        {
            template(typename Rng, typename Pred)(requires viewable_range<Rng> AND input_range<Rng> AND predicate<Pred &, iterator_t<Rng>> AND copy_constructible<Pred>)
            constexpr iter_take_while_view<all_t<Rng>, Pred> //
            operator()(Rng && rng, Pred pred) const
            {
                return {all(static_cast<Rng &&>(rng)), eastl::move(pred)};
            }
        };

        struct iter_take_while_fn : iter_take_while_base_fn
        {
            using iter_take_while_base_fn::operator();

            template<typename Pred>
            constexpr auto operator()(Pred pred) const
            {
                return make_view_closure(bind_back(iter_take_while_base_fn{}, eastl::move(pred)));
            }
        };

        struct take_while_base_fn
        {
            template(typename Rng, typename Pred)(requires viewable_range<Rng> AND input_range<Rng> AND indirect_unary_predicate<Pred &, iterator_t<Rng>>)
            constexpr take_while_view<all_t<Rng>, Pred> //
            operator()(Rng && rng, Pred pred) const
            {
                return {all(static_cast<Rng &&>(rng)), eastl::move(pred)};
            }
            template(typename Rng, typename Pred, typename Proj)(requires viewable_range<Rng> AND input_range<Rng> AND indirect_unary_predicate<composed<Pred, Proj> &, iterator_t<Rng>>)
            constexpr take_while_view<all_t<Rng>, composed<Pred, Proj>> //
            operator()(Rng && rng, Pred pred, Proj proj) const
            {
                return {all(static_cast<Rng &&>(rng)),compose(eastl::move(pred), eastl::move(proj))};
            }
        };

        struct take_while_bind_fn
        {
            template<typename Pred>
            constexpr auto operator()(Pred pred) const // TODO: underconstrained
            {
                return make_view_closure(bind_back(take_while_base_fn{}, eastl::move(pred)));
            }
            template(typename Pred, typename Proj)(requires (!range<Pred>)) // TODO: underconstrained
            constexpr auto operator()(Pred && pred, Proj proj) const
                                                          
            {
                return make_view_closure(bind_back(take_while_base_fn{}, static_cast<Pred &&>(pred), eastl::move(proj)));
            }
        };

        struct EARANGES_EMPTY_BASES take_while_fn : take_while_base_fn, take_while_bind_fn
        {
            using take_while_base_fn::operator();
            using take_while_bind_fn::operator();
        };

        /// \relates iter_take_while_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(iter_take_while_fn, iter_take_while)

        /// \relates take_while_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(take_while_fn, take_while)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::iter_take_while_view)
EARANGES_SATISFY_BOOST_RANGE(::ranges::take_while_view)

#endif