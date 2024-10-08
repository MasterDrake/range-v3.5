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

#ifndef EARANGES_VIEW_SPLIT_WHEN_HPP
#define EARANGES_VIEW_SPLIT_WHEN_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/find_if_not.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/indirect.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/take_while.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    template<typename Rng, typename Fun>
    struct split_when_view
      : view_facade<split_when_view<Rng, Fun>,
                    is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        Rng rng_;
        semiregular_box_t<Fun> fun_;

        template<bool IsConst>
        struct cursor
        {
        private:
            friend range_access;
            friend split_when_view;
            friend struct cursor<!IsConst>;
            bool zero_;
            using CRng = meta::const_if_c<IsConst, Rng>;
            iterator_t<CRng> cur_;
            sentinel_t<CRng> last_;
            using fun_ref_t = semiregular_box_ref_or_val_t<Fun, IsConst>;
            fun_ref_t fun_;

            struct search_pred
            {
                bool zero_;
                iterator_t<CRng> first_;
                sentinel_t<CRng> last_;
                fun_ref_t fun_;
                bool operator()(iterator_t<CRng> cur) const
                {
                    return (zero_ && cur == first_) ||
                           (cur != last_ && !invoke(fun_, cur, last_).first);
                }
            };
            using reference_ =
                indirect_view<take_while_view<iota_view<iterator_t<CRng>>, search_pred>>;
            reference_ read() const
            {
                return reference_{{views::iota(cur_), {zero_, cur_, last_, fun_}}};
            }
            void next()
            {
                EARANGES_EXPECT(cur_ != last_);
                // If the last match consumed zero elements, bump the position.
                if(zero_)
                {
                    zero_ = false;
                    ++cur_;
                }
                for(; cur_ != last_; ++cur_)
                {
                    auto p = invoke(fun_, cur_, last_);
                    if(p.first)
                    {
                        zero_ = (cur_ == p.second);
                        cur_ = p.second;
                        return;
                    }
                }
            }
            bool equal(default_sentinel_t) const
            {
                return cur_ == last_;
            }
            bool equal(cursor const & that) const
            {
                return cur_ == that.cur_;
            }
            cursor(fun_ref_t fun, iterator_t<CRng> first, sentinel_t<CRng> last)
              : cur_(first)
              , last_(last)
              , fun_(fun)
            {
                // For skipping an initial zero-length match
                auto p = invoke(fun, first, last);
                zero_ = p.first && first == p.second;
            }

        public:
            cursor() = default;
            template(bool Other)(
                requires IsConst AND CPP_NOT(Other)) //
            cursor(cursor<Other> that)
              : cursor{eastl::move(that.cur_), eastl::move(that.last_), eastl::move(that.fun_)}
            {}
        };
        cursor<false> begin_cursor()
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }
        template(bool Const = true)(
            requires Const AND range<meta::const_if_c<Const, Rng>> AND
                invocable<Fun const &, iterator_t<meta::const_if_c<Const, Rng>>,
                          sentinel_t<meta::const_if_c<Const, Rng>>>)
        cursor<Const> begin_cursor() const
        {
            return {fun_, ranges::begin(rng_), ranges::end(rng_)};
        }

    public:
        split_when_view() = default;
        split_when_view(Rng rng, Fun fun)
          : rng_(eastl::move(rng))
          , fun_(eastl::move(fun))
        {}
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template(typename Rng, typename Fun)(requires copy_constructible<Fun>)
    split_when_view(Rng &&, Fun) -> split_when_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct split_when_base_fn
        {
        private:
            template<typename Pred>
            struct predicate_pred_
            {
                semiregular_box_t<Pred> pred_;

                template(typename I, typename S)(requires sentinel_for<S, I>)
                eastl::pair<bool, I> operator()(I cur, S last) const
                {
                    auto where = ranges::find_if_not(cur, last, eastl::ref(pred_));
                    return {cur != where, where};
                }
            };

        public:
            template(typename Rng, typename Fun)(
                requires viewable_range<Rng> AND forward_range<Rng> AND
                    invocable<Fun &, iterator_t<Rng>, sentinel_t<Rng>> AND
                    invocable<Fun &, iterator_t<Rng>, iterator_t<Rng>> AND
                    copy_constructible<Fun> AND
                    convertible_to<
                        invoke_result_t<Fun &, iterator_t<Rng>, sentinel_t<Rng>>,
                        eastl::pair<bool, iterator_t<Rng>>>)
            split_when_view<all_t<Rng>, Fun> operator()(Rng && rng, Fun fun) const //
            {
                return {all(static_cast<Rng &&>(rng)), eastl::move(fun)};
            }
            template(typename Rng, typename Fun)(
                requires viewable_range<Rng> AND forward_range<Rng> AND
                    predicate<Fun const &, range_reference_t<Rng>> AND
                    copy_constructible<Fun>)
            split_when_view<all_t<Rng>, predicate_pred_<Fun>> //
            operator()(Rng && rng, Fun fun) const
            {
                return {all(static_cast<Rng &&>(rng)), predicate_pred_<Fun>{eastl::move(fun)}};
            }
        };

        struct split_when_fn : split_when_base_fn
        {
            using split_when_base_fn::operator();

            template<typename T>
            constexpr auto operator()(T && t) const
            {
                return make_view_closure(bind_back(split_when_base_fn{}, static_cast<T &&>(t)));
            }
        };

        /// \relates split_when_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(split_when_fn, split_when)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::split_when_view)

#endif