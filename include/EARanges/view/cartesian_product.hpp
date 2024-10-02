/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014.
//  Copyright Casey Carter 2017.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_CARTESIAN_PRODUCT_HPP
#define EARANGES_VIEW_CARTESIAN_PRODUCT_HPP

#include <cstdint>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/utility/tuple_algorithm.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/empty.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/view.hpp> // for dereference_f

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename State, typename Value>
        using product_cardinality = eastl::integral_constant<
            cardinality,
            State::value == 0 || Value::value == 0
                ? static_cast<cardinality>(0)
                : State::value == unknown || Value::value == unknown
                      ? unknown
                      : State::value == infinite || Value::value == infinite
                            ? infinite
                            : State::value == finite || Value::value == finite
                                  ? finite
                                  : static_cast<cardinality>(
                                        State::value * Value::value)>;

        struct cartesian_size_fn
        {
            template(typename Size, typename Rng)(
                requires integer_like_<Size> AND sized_range<Rng> AND
                    common_with<Size, range_size_t<Rng>>)
            common_type_t<Size, range_size_t<Rng>> operator()(Size s, Rng && rng) const
            {
                using S = common_type_t<Size, range_size_t<Rng>>;
                return static_cast<S>(s) * static_cast<S>(ranges::size(rng));
            }
        };

        template<typename... Views>
        using cartesian_product_cardinality =
            meta::fold<meta::list<range_cardinality<Views>...>,
                       eastl::integral_constant<cardinality, static_cast<cardinality>((sizeof...(Views) > 0))>,
                       meta::quote<detail::product_cardinality>>;
    } // namespace detail
      /// \endcond

    /// \addtogroup group-views
    /// @{

    // clang-format off
    /// \concept cartesian_produce_view_can_const
    /// \brief The \c cartesian_produce_view_can_const concept
    template<typename...Views>
    CPP_concept cartesian_produce_view_can_const =
        and_v<range<Views const>...>;

    /// \concept cartesian_produce_view_can_size_
    /// \brief The \c cartesian_produce_view_can_size_ concept
    template(typename IsConst, typename... Views)(
    concept (cartesian_produce_view_can_size_)(IsConst, Views...),
        and_v<common_with<std::uintmax_t, range_size_t<meta::const_if<IsConst, Views>>>...>
    );
    /// \concept cartesian_produce_view_can_size
    /// \brief The \c cartesian_produce_view_can_size concept
    template<typename IsConst, typename...Views>
    CPP_concept cartesian_produce_view_can_size =
        and_v<sized_range<meta::const_if<IsConst, Views>>...> &&
        CPP_concept_ref(ranges::cartesian_produce_view_can_size_, IsConst, Views...);

    /// \concept cartesian_produce_view_can_distance_
    /// \brief The \c cartesian_produce_view_can_distance_ concept
    template(typename IsConst, typename... Views)(
    concept (cartesian_produce_view_can_distance_)(IsConst, Views...),
        and_v<sized_sentinel_for<
            iterator_t<meta::const_if<IsConst, Views>>,
            iterator_t<meta::const_if<IsConst, Views>>>...>
    );
    /// \concept cartesian_produce_view_can_distance
    /// \brief The \c cartesian_produce_view_can_distance concept
    template<typename IsConst, typename...Views>
    CPP_concept cartesian_produce_view_can_distance =
        cartesian_produce_view_can_size<IsConst, Views...> &&
        CPP_concept_ref(ranges::cartesian_produce_view_can_distance_, IsConst, Views...);

    /// \concept cartesian_produce_view_can_random_
    /// \brief The \c cartesian_produce_view_can_random_ concept
    template(typename IsConst, typename... Views)(
    concept (cartesian_produce_view_can_random_)(IsConst, Views...),
        and_v<random_access_iterator<iterator_t<meta::const_if<IsConst, Views>>>...>
    );
    /// \concept cartesian_produce_view_can_random
    /// \brief The \c cartesian_produce_view_can_random concept
    template<typename IsConst, typename...Views>
    CPP_concept cartesian_produce_view_can_random =
        cartesian_produce_view_can_distance<IsConst, Views...> &&
        CPP_concept_ref(ranges::cartesian_produce_view_can_random_, IsConst, Views...);

    /// \concept cartesian_produce_view_can_bidi_
    /// \brief The \c cartesian_produce_view_can_bidi_ concept
    template(typename IsConst, typename... Views)(
    concept (cartesian_produce_view_can_bidi_)(IsConst, Views...),
        and_v<common_range<meta::const_if<IsConst, Views>>...,
            bidirectional_iterator<iterator_t<meta::const_if<IsConst, Views>>>...>
    );
    /// \concept cartesian_produce_view_can_bidi
    /// \brief The \c cartesian_produce_view_can_bidi concept
    template<typename IsConst, typename...Views>
    CPP_concept cartesian_produce_view_can_bidi =
        cartesian_produce_view_can_random<IsConst, Views...> ||
        CPP_concept_ref(ranges::cartesian_produce_view_can_bidi_, IsConst, Views...);
    // clang-format on

    template<typename... Views>
    struct cartesian_product_view
      : view_facade<cartesian_product_view<Views...>,
                    detail::cartesian_product_cardinality<Views...>::value>
    {
    private:
        friend range_access;
        CPP_assert(and_v<(forward_range<Views> && view_<Views>)...>);
        CPP_assert(sizeof...(Views) != 0);

        static constexpr auto my_cardinality =
            detail::cartesian_product_cardinality<Views...>::value;

        eastl::tuple<Views...> views_;

        template<bool IsConst_>
        struct cursor
        {
        private:
            using IsConst = meta::bool_<IsConst_>;
            friend cursor<true>;
            template<typename T>
            using constify_if = meta::const_if_c<IsConst_, T>;
            using difference_type =
                common_type_t<std::intmax_t, range_difference_t<Views>...>;

            constify_if<cartesian_product_view> * view_;
            eastl::tuple<iterator_t<constify_if<Views>>...> its_;

            void next_(meta::size_t<1>)
            {
                auto & v = eastl::get<0>(view_->views_);
                auto & i = eastl::get<0>(its_);
                auto const last = ranges::end(v);
                EARANGES_EXPECT(i != last);
                ++i;
            }
            template<eastl::size_t N>
            void next_(meta::size_t<N>)
            {
                auto & v = eastl::get<N - 1>(view_->views_);
                auto & i = eastl::get<N - 1>(its_);
                auto const last = ranges::end(v);
                EARANGES_EXPECT(i != last);
                if(++i == last)
                {
                    i = ranges::begin(v);
                    next_(meta::size_t<N - 1>{});
                }
            }
            void prev_(meta::size_t<0>)
            {
                EARANGES_EXPECT(false);
            }
            template<eastl::size_t N>
            void prev_(meta::size_t<N>)
            {
                auto & v = eastl::get<N - 1>(view_->views_);
                auto & i = eastl::get<N - 1>(its_);
                if(i == ranges::begin(v))
                {
                    CPP_assert(cartesian_produce_view_can_bidi<IsConst, Views...>);
                    // cartesian_produce_view_can_bidi<IsConst, Views...> implies this
                    // advance call is O(1)
                    ranges::advance(i, ranges::end(v));
                    prev_(meta::size_t<N - 1>{});
                }
                --i;
            }
            bool equal_(cursor const &, meta::size_t<0>) const
            {
                return true;
            }
            template<eastl::size_t N>
            bool equal_(cursor const & that, meta::size_t<N>) const
            {
                return eastl::get<N - 1>(its_) == eastl::get<N - 1>(that.its_) &&
                       equal_(that, meta::size_t<N - 1>{});
            }
            difference_type distance_(cursor const & that, meta::size_t<1>) const
            {
                return difference_type{eastl::get<0>(that.its_) - eastl::get<0>(its_)};
            }
            template<eastl::size_t N>
            difference_type distance_(cursor const & that, meta::size_t<N>) const
            {
                difference_type const d = distance_(that, meta::size_t<N - 1>{});
                auto const scale = ranges::distance(eastl::get<N - 1>(view_->views_));
                auto const increment = eastl::get<N - 1>(that.its_) - eastl::get<N - 1>(its_);
                return difference_type{d * scale + increment};
            }
            void advance_(meta::size_t<0>, difference_type)
            {
                EARANGES_EXPECT(false);
            }
            EARANGES_DIAGNOSTIC_PUSH
            EARANGES_DIAGNOSTIC_IGNORE_DIVIDE_BY_ZERO
            template<eastl::size_t N>
            void advance_(meta::size_t<N>, difference_type n)
            {
                if(n == 0)
                    return;

                auto & i = eastl::get<N - 1>(its_);
                auto const my_size = static_cast<difference_type>(
                    ranges::size(eastl::get<N - 1>(view_->views_)));
                auto const first = ranges::begin(eastl::get<N - 1>(view_->views_));

                auto const idx = static_cast<difference_type>(i - first);
                EARANGES_EXPECT(0 <= idx);
                EARANGES_EXPECT(idx < my_size || (N == 1 && idx == my_size && n < 0));
                EARANGES_EXPECT(n < INTMAX_MAX - idx);
                n += idx;

                auto n_div = n / my_size;
                auto n_mod = n % my_size;

                if(EARANGES_CONSTEXPR_IF(N != 1))
                {
                    if(n_mod < 0)
                    {
                        n_mod += my_size;
                        --n_div;
                    }
                    advance_(meta::size_t<N - 1>{}, n_div);
                }
                EARANGES_EXPECT(0 <= n_mod && n_mod < my_size);

                if(EARANGES_CONSTEXPR_IF(N == 1))
                {
                    if(n_div > 0)
                    {
                        EARANGES_EXPECT(n_div == 1);
                        EARANGES_EXPECT(n_mod == 0);
                        n_mod = my_size;
                    }
                    else if(n_div < 0)
                    {
                        EARANGES_EXPECT(n_div == -1);
                        EARANGES_EXPECT(n_mod == 0);
                    }
                }

                using D = iter_difference_t<decltype(first)>;
                i = first + static_cast<D>(n_mod);
            }
            EARANGES_DIAGNOSTIC_POP
            void check_at_end_(meta::size_t<1>, bool at_end = false)
            {
                if(at_end)
                    ranges::advance(eastl::get<0>(its_), ranges::end(eastl::get<0>(view_->views_)));
            }
            template<eastl::size_t N>
            void check_at_end_(meta::size_t<N>, bool at_end = false)
            {
                return check_at_end_(
                    meta::size_t<N - 1>{},
                    at_end || bool(eastl::get<N - 1>(its_) == ranges::end(eastl::get<N - 1>(view_->views_))));
            }
            cursor(end_tag, constify_if<cartesian_product_view> * view, eastl::true_type) // common_with
              : cursor(begin_tag{}, view)
            {
                CPP_assert(common_range<meta::at_c<meta::list<constify_if<Views>...>, 0>>);
                eastl::get<0>(its_) = ranges::end(eastl::get<0>(view->views_));
            }
            cursor(end_tag, constify_if<cartesian_product_view> * view, eastl::false_type) // !common_with
              : cursor(begin_tag{}, view)
            {
                using View0 = meta::at_c<meta::list<constify_if<Views>...>, 0>;
                CPP_assert(!common_range<View0> && random_access_range<View0> && sized_range<View0>);
                eastl::get<0>(its_) += ranges::distance(eastl::get<0>(view->views_));
            }

        public:
            using value_type = eastl::tuple<range_value_t<Views>...>;

            cursor() = default;
            explicit cursor(begin_tag, constify_if<cartesian_product_view> * view)
              : view_(view)
              , its_(tuple_transform(view->views_, ranges::begin))
            {
                // If any of the constituent views is empty, the cartesian_product is
                // empty and this "begin" iterator needs to become an "end" iterator.
                check_at_end_(meta::size_t<sizeof...(Views)>{});
            }
            explicit cursor(end_tag, constify_if<cartesian_product_view> * view)
              : cursor(
                    end_tag{}, view,
                    meta::bool_<
                        common_range<meta::at_c<meta::list<constify_if<Views>...>, 0>>>{})
            {}
            template(bool Other)(
                requires IsConst_ AND CPP_NOT(Other)) //
            cursor(cursor<Other> that)
              : view_(that.view_)
              , its_(eastl::move(that.its_))
            {}
            common_tuple<range_reference_t<constify_if<Views>>...> read() const
            {
                return tuple_transform(its_, detail::dereference_fn{});
            }
            void next()
            {
                next_(meta::size_t<sizeof...(Views)>{});
            }
            bool equal(default_sentinel_t) const
            {
                return eastl::get<0>(its_) == ranges::end(eastl::get<0>(view_->views_));
            }
            bool equal(cursor const & that) const
            {
                return equal_(that, meta::size_t<sizeof...(Views)>{});
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires cartesian_produce_view_can_bidi<IsConst, Views...>)
            {
                prev_(meta::size_t<sizeof...(Views)>{});
            }
            CPP_auto_member
            auto CPP_fun(distance_to)(cursor const & that)(
                const requires cartesian_produce_view_can_distance<IsConst, Views...>)
            {
                return distance_(that, meta::size_t<sizeof...(Views)>{});
            }
            CPP_member
            auto advance(difference_type n) //
                -> CPP_ret(void)(requires cartesian_produce_view_can_random<IsConst, Views...>)
            {
                advance_(meta::size_t<sizeof...(Views)>{}, n);
            }
        };
        cursor<false> begin_cursor()
        {
            return cursor<false>{begin_tag{}, this};
        }
        CPP_member
        auto begin_cursor() const //
            -> CPP_ret(cursor<true>)(requires cartesian_produce_view_can_const<Views...>)
        {
            return cursor<true>{begin_tag{}, this};
        }
        CPP_member
        auto end_cursor() //
            -> CPP_ret(cursor<false>)(requires cartesian_produce_view_can_bidi<eastl::false_type, Views...>)
        {
            return cursor<false>{end_tag{}, this};
        }
        CPP_member
        auto end_cursor() const //
            -> CPP_ret(cursor<true>)(requires cartesian_produce_view_can_bidi<eastl::true_type, Views...>)
        {
            return cursor<true>{end_tag{}, this};
        }
        CPP_member
        auto end_cursor() const //
            -> CPP_ret(default_sentinel_t)(requires (!cartesian_produce_view_can_bidi<eastl::true_type, Views...>))
        {
            return {};
        }

    public:
        cartesian_product_view() = default;
        constexpr explicit cartesian_product_view(Views... views)
          : views_{detail::move(views)...}
        {}
        template(typename...)(requires (my_cardinality >= 0)) //
        static constexpr eastl::size_t size() noexcept
        {
            return eastl::size_t{my_cardinality};
        }
        CPP_auto_member
        auto CPP_fun(size)()(const //
            requires (my_cardinality < 0) &&
                cartesian_produce_view_can_size<eastl::true_type, Views...>)
        {
            return tuple_foldl(views_, std::uintmax_t{1}, detail::cartesian_size_fn{});
        }
        CPP_auto_member
        auto CPP_fun(size)()(
            requires (my_cardinality < 0) &&
                cartesian_produce_view_can_size<eastl::false_type, Views...>)
        {
            return tuple_foldl(views_, std::uintmax_t{1}, detail::cartesian_size_fn{});
        }
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename... Rng>
    cartesian_product_view(Rng &&...) //
        -> cartesian_product_view<views::all_t<Rng>...>;
#endif

    namespace views
    {
        struct cartesian_product_fn
        {
            constexpr empty_view<eastl::tuple<>> operator()() const noexcept
            {
                return {};
            }
            template(typename... Rngs)(
                requires (sizeof...(Rngs) != 0) AND
                concepts::and_v<(forward_range<Rngs> && viewable_range<Rngs>)...>)
            constexpr cartesian_product_view<all_t<Rngs>...> operator()(Rngs &&... rngs)
                const
            {
                return cartesian_product_view<all_t<Rngs>...>{
                    all(static_cast<Rngs &&>(rngs))...};
            }
#if defined(_MSC_VER)
            template(typename Rng0)(
                requires forward_range<Rng0> AND viewable_range<Rng0>)
            constexpr cartesian_product_view<all_t<Rng0>> operator()(Rng0 && rng0) const
            {
                return cartesian_product_view<all_t<Rng0>>{
                    all(static_cast<Rng0 &&>(rng0))};
            }
            template(typename Rng0, typename Rng1)(
                requires forward_range<Rng0> AND viewable_range<Rng0> AND
                             forward_range<Rng1> AND viewable_range<Rng1>)
            constexpr cartesian_product_view<all_t<Rng0>, all_t<Rng1>> //
            operator()(Rng0 && rng0, Rng1 && rng1) const
            {
                return cartesian_product_view<all_t<Rng0>, all_t<Rng1>>{
                    all(static_cast<Rng0 &&>(rng0)), //
                    all(static_cast<Rng1 &&>(rng1))};
            }
            template(typename Rng0, typename Rng1, typename Rng2)(
                requires forward_range<Rng0> AND viewable_range<Rng0> AND
                    forward_range<Rng1> AND viewable_range<Rng1> AND
                    forward_range<Rng2> AND viewable_range<Rng2>)
            constexpr cartesian_product_view<all_t<Rng0>, all_t<Rng1>, all_t<Rng2>> //
            operator()(Rng0 && rng0, Rng1 && rng1, Rng2 && rng2) const
            {
                return cartesian_product_view<all_t<Rng0>, all_t<Rng1>, all_t<Rng2>>{
                    all(static_cast<Rng0 &&>(rng0)), //
                    all(static_cast<Rng1 &&>(rng1)), //
                    all(static_cast<Rng2 &&>(rng2))};
            }
#endif
        };

        EARANGES_INLINE_VARIABLE(cartesian_product_fn, cartesian_product)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif