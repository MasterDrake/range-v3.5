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

#ifndef EARANGES_VIEW_PARTIAL_SUM_HPP
#define EARANGES_VIEW_PARTIAL_SUM_HPP

#include <EASTL/iterator.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/arithmetic.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/addressof.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        // clang-format off
        /// \concept partial_sum_view_constraints_
        /// \brief The \c partial_sum_view_constraints_ concept
        template(typename Rng, typename Fun)(
        concept (partial_sum_view_constraints_)(Rng, Fun),
            copy_constructible<range_value_t<Rng>> AND
            constructible_from<range_value_t<Rng>, range_reference_t<Rng>> AND
            assignable_from<range_value_t<Rng> &, range_reference_t<Rng>> AND
            indirectly_binary_invocable_<Fun &, iterator_t<Rng>, iterator_t<Rng>> AND
            assignable_from<
                range_value_t<Rng> &,
                indirect_result_t<Fun &, iterator_t<Rng>, iterator_t<Rng>>>
        );
        /// \concept partial_sum_view_constraints
        /// \brief The \c partial_sum_view_constraints concept
        template<typename Rng, typename Fun>
        CPP_concept partial_sum_view_constraints =
            input_range<Rng> &&
            copy_constructible<Fun> &&
            CPP_concept_ref(detail::partial_sum_view_constraints_, Rng, Fun);
        // clang-format on
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Fun>
    struct partial_sum_view
      : view_facade<partial_sum_view<Rng, Fun>, range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        CPP_assert(view_<Rng>);
        CPP_assert(detail::partial_sum_view_constraints<Rng, Fun>);

        EARANGES_NO_UNIQUE_ADDRESS Rng base_{};
        EARANGES_NO_UNIQUE_ADDRESS semiregular_box_t<Fun> fun_;

        template<bool IsConst>
        struct cursor
        {
        private:
            friend cursor<true>;

            using Parent = meta::const_if_c<IsConst, partial_sum_view>;
            using Base = meta::const_if_c<IsConst, Rng>;

            Parent * parent_ = nullptr;
            EARANGES_NO_UNIQUE_ADDRESS iterator_t<Base> current_{};
            EARANGES_NO_UNIQUE_ADDRESS semiregular_box_t<range_value_t<Rng>> sum_;

        public:
            using single_pass = meta::bool_<single_pass_iterator_<iterator_t<Base>>>;

            cursor() = default;
            constexpr explicit cursor(Parent * rng)
              : parent_{rng}
              , current_(ranges::begin(rng->base_))
            {
                if(current_ != ranges::end(rng->base_))
                    sum_ = *current_;
            }
            template(bool Other)(
                requires IsConst AND CPP_NOT(Other) AND
                convertible_to<iterator_t<Rng> const &,
                               iterator_t<Base>>)
            constexpr cursor(cursor<Other> const & that)
              : parent_{that.parent_}
              , current_(that.current_)
              , sum_(that.sum_)
            {}
            constexpr range_value_t<Rng> read() const
            {
                EARANGES_EXPECT(current_ != ranges::end(parent_->base_));
                return sum_;
            }
            constexpr void next()
            {
                auto last = ranges::end(parent_->base_);
                EARANGES_EXPECT(current_ != last);
                if(++current_ != last)
                {
                    auto & sum = static_cast<range_value_t<Rng> &>(sum_);
                    using F = meta::const_if_c<IsConst, Fun>;
                    auto & f = static_cast<F &>(parent_->fun_);
                    sum = invoke(f, sum, *current_);
                }
            }
            constexpr bool equal(default_sentinel_t) const
            {
                return current_ == ranges::end(parent_->base_);
            }
            CPP_auto_member
            constexpr bool CPP_fun(equal)(cursor const & that)(const //
                requires equality_comparable<iterator_t<Base>>)
            {
                EARANGES_EXPECT(parent_ == that.parent_);
                return current_ == that.current_;
            }
        };

        constexpr cursor<false> begin_cursor()
        {
            return cursor<false>{this};
        }
        template(typename CRng = Rng const)(
            requires detail::partial_sum_view_constraints<CRng, Fun const>)
        constexpr cursor<true> begin_cursor() const
        {
            return cursor<true>{this};
        }

    public:
        partial_sum_view() = default;
        constexpr partial_sum_view(Rng rng, Fun fun) noexcept(
            eastl::is_nothrow_move_constructible<Rng>::value &&
                eastl::is_nothrow_move_constructible<Fun>::value)
          : base_(eastl::move(rng))
          , fun_(eastl::move(fun))
        {}
        CPP_auto_member
        constexpr auto CPP_fun(size)()(
            requires sized_range<Rng>)
        {
            return ranges::size(base_);
        }
        CPP_auto_member
        constexpr auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            return ranges::size(base_);
        }
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template(typename Rng, typename Fun)(
        requires copy_constructible<Fun>)
    partial_sum_view(Rng &&, Fun)
        -> partial_sum_view<views::all_t<Rng>, Fun>;
#endif

    namespace views
    {
        struct partial_sum_base_fn
        {
            template(typename Rng, typename Fun = plus)(
                requires detail::partial_sum_view_constraints<all_t<Rng>, Fun>)
            constexpr partial_sum_view<all_t<Rng>, Fun> //
            operator()(Rng && rng, Fun fun = {}) const
            {
                return {all(static_cast<Rng &&>(rng)), eastl::move(fun)};
            }
        };

        struct partial_sum_fn : partial_sum_base_fn
        {
            using partial_sum_base_fn::operator();

            template(typename Fun)(requires (!range<Fun>))
            constexpr auto operator()(Fun && fun) const
            {
                return make_view_closure(bind_back(partial_sum_base_fn{}, static_cast<Fun &&>(fun)));
            }
        };

        /// \relates partial_sum_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(view_closure<partial_sum_fn>, partial_sum)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::partial_sum_view)

#endif