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

#ifndef EARANGES_VIEW_ZIP_WITH_HPP
#define EARANGES_VIEW_ZIP_WITH_HPP

#include <EASTL/numeric_limits.h>
#include <EASTL/type_traits.h>
#include <EASTL/tuple.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/indirect.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/common_type.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/utility/tuple_algorithm.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/empty.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \cond
        namespace detail
        {
            struct equal_to_
            {
                template<typename T, typename U>
                bool operator()(T const & t, U const & u) const
                {
                    return static_cast<bool>(t == u);
                }
            };
            EARANGES_INLINE_VARIABLE(equal_to_, equal_to)

            struct dec_
            {
                template<typename T>
                void operator()(T & t) const
                {
                    --t;
                }
            };
            EARANGES_INLINE_VARIABLE(dec_, dec)

            struct inc_
            {
                template<typename T>
                void operator()(T & t) const
                {
                    ++t;
                }
            };
            EARANGES_INLINE_VARIABLE(inc_, inc)

            struct _advance_
            {
                template(typename I, typename Diff)(
                    requires input_or_output_iterator<I> AND integer_like_<Diff>) void
                operator()(I & i, Diff n) const
                {
                    advance(i, static_cast<iter_difference_t<I>>(n));
                }
            };
            EARANGES_INLINE_VARIABLE(_advance_, advance_)

            struct distance_to_
            {
                template<typename T>
                constexpr auto operator()(T const & t, T const & u) const
                    -> decltype(u - t)
                {
                    return u - t;
                }
            };
            EARANGES_INLINE_VARIABLE(distance_to_, distance_to)

            struct _min_
            {
                template<typename T, typename U>
                constexpr auto operator()(T const & t, U const & u) const
                    -> decltype(true ? t : u)
                {
                    return u < t ? u : t;
                }
            };
            EARANGES_INLINE_VARIABLE(_min_, min_)

            struct _max_
            {
                template<typename T, typename U>
                constexpr auto operator()(T const & t, U const & u) const
                    -> decltype(true ? u : t)
                {
                    return u < t ? t : u;
                }
            };
            EARANGES_INLINE_VARIABLE(_max_, max_)

            template<typename State, typename Value>
            using zip_cardinality = eastl::integral_constant<
                cardinality,
                State::value >= 0 && Value::value >= 0 ? min_(State::value, Value::value)
                : State::value >= 0 && Value::value == infinite      ? State::value
                : State::value == infinite && Value::value >= 0      ? Value::value
                : State::value == finite || Value::value == finite   ? finite
                : State::value == unknown || Value::value == unknown ? unknown
                                                                     : infinite>;
        } // namespace detail
        /// \endcond

        namespace views
        {
            // clang-format off
        /// \concept zippable_with_
        /// \brief The \c zippable_with_ concept
        template(typename Fun, typename... Rngs)(
        concept (zippable_with_)(Fun, Rngs...),
            invocable<Fun&, iterator_t<Rngs>...> AND
            invocable<Fun&, copy_tag, iterator_t<Rngs>...> AND
            invocable<Fun&, move_tag, iterator_t<Rngs>...>
        );
        /// \concept zippable_with
        /// \brief The \c zippable_with concept
        template<typename Fun, typename ...Rngs>
        CPP_concept zippable_with =
            and_v<input_range<Rngs>...> &&
            copy_constructible<Fun> &&
            CPP_concept_ref(views::zippable_with_, Fun, Rngs...);
            // clang-format on
        } // namespace views

        /// \addtogroup group-views
        /// @{
        template<typename Fun, typename... Rngs>
        struct iter_zip_with_view
          : view_facade<
                iter_zip_with_view<Fun, Rngs...>,
                meta::fold<meta::list<range_cardinality<Rngs>...>,
                           eastl::integral_constant<cardinality, cardinality::infinite>,
                           meta::quote<detail::zip_cardinality>>::value>
        {
        private:
            CPP_assert(sizeof...(Rngs) != 0);
            friend range_access;

            semiregular_box_t<Fun> fun_;
            eastl::tuple<Rngs...> rngs_;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;

            template<bool Const>
            struct cursor;

            template<bool Const>
            struct sentinel
            {
            private:
                friend struct cursor<Const>;
                friend struct sentinel<!Const>;
                eastl::tuple<sentinel_t<meta::const_if_c<Const, Rngs>>...> ends_;

            public:
                sentinel() = default;
                sentinel(detail::ignore_t,
                         eastl::tuple<sentinel_t<meta::const_if_c<Const, Rngs>>...> ends)
                  : ends_(eastl::move(ends))
                {}
                template(bool Other)(requires Const AND CPP_NOT(Other)) //
                    sentinel(sentinel<Other> that)
                  : ends_(eastl::move(that.ends_))
                {}
            };

            template<bool Const>
            struct cursor
            {
            private:
                friend struct cursor<!Const>;
                using fun_ref_ = semiregular_box_ref_or_val_t<Fun, Const>;
                fun_ref_ fun_;
                eastl::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its_;

            public:
                using difference_type =
                    common_type_t<range_difference_t<meta::const_if_c<Const, Rngs>>...>;
                using single_pass = meta::or_c<(bool)single_pass_iterator_<
                    iterator_t<meta::const_if_c<Const, Rngs>>>...>;
                using value_type = detail::decay_t<invoke_result_t<
                    fun_ref_ &, copy_tag, iterator_t<meta::const_if_c<Const, Rngs>>...>>;

                cursor() = default;
                cursor(fun_ref_ fun,
                       eastl::tuple<iterator_t<meta::const_if_c<Const, Rngs>>...> its)
                  : fun_(eastl::move(fun))
                  , its_(eastl::move(its))
                {}
                template(bool Other)(requires Const AND CPP_NOT(Other)) //
                    cursor(cursor<Other> that)
                  : fun_(eastl::move(that.fun_))
                  , its_(eastl::move(that.its_))
                {}
                // clang-format off
            auto CPP_auto_fun(read)()(const)
            (
                return tuple_apply(fun_, its_)
            )
                    // clang-format on
                    void next()
                {
                    tuple_for_each(its_, detail::inc);
                }
                CPP_member
                auto equal(cursor const & that) const //
                    -> CPP_ret(bool)(
                        requires and_v<
                            sentinel_for<iterator_t<meta::const_if_c<Const, Rngs>>,
                                         iterator_t<meta::const_if_c<Const, Rngs>>>...>)
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the last.
                    return tuple_foldl(tuple_transform(its_, that.its_, detail::equal_to),
                                       false,
                                       [](bool a, bool b) { return a || b; });
                }
                bool equal(sentinel<Const> const & s) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the last.
                    return tuple_foldl(tuple_transform(its_, s.ends_, detail::equal_to),
                                       false,
                                       [](bool a, bool b) { return a || b; });
                }
                CPP_member
                auto prev() //
                    -> CPP_ret(void)(
                        requires and_v<
                            bidirectional_range<meta::const_if_c<Const, Rngs>>...>)
                {
                    tuple_for_each(its_, detail::dec);
                }
                CPP_member
                auto advance(difference_type n) //
                    -> CPP_ret(void)(
                        requires and_v<
                            random_access_range<meta::const_if_c<Const, Rngs>>...>)
                {
                    tuple_for_each(its_, bind_back(detail::advance_, n));
                }
                CPP_member
                auto distance_to(cursor const & that) const //
                    -> CPP_ret(difference_type)(
                        requires and_v<sized_sentinel_for<
                            iterator_t<meta::const_if_c<Const, Rngs>>,
                            iterator_t<meta::const_if_c<Const, Rngs>>>...>)
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accommodate zippers of sequences of different
                    // length.
                    auto first_size = eastl::get<0>(that.its_) - eastl::get<0>(its_);
                    if(first_size == 0)
                        return static_cast<difference_type>(0);

                    if(0 < first_size)
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (eastl::numeric_limits<difference_type>::max)(),
                            detail::min_);
                    else
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (eastl::numeric_limits<difference_type>::min)(),
                            detail::max_);
                }
                // clang-format off
            template<std::size_t... Is>
            auto CPP_auto_fun(move_)(meta::index_sequence<Is...>)(const)
            (
                return invoke(fun_, move_tag{}, eastl::get<Is>(its_)...)
            )
                    // clang-format on
                    auto move() const
                    noexcept(noexcept(eastl::declval<cursor const &>().move_(
                        meta::make_index_sequence<sizeof...(Rngs)>{})))
                        -> decltype(eastl::declval<cursor const &>().move_(
                            meta::make_index_sequence<sizeof...(Rngs)>{}))
                {
                    return move_(meta::make_index_sequence<sizeof...(Rngs)>{});
                }
            };

            template<bool Const>
            using end_cursor_t = meta::if_c<
                concepts::and_v<(bool)common_range<Rngs>...,
                                !(bool)single_pass_iterator_<iterator_t<Rngs>>...>,
                cursor<Const>, sentinel<Const>>;

            cursor<false> begin_cursor()
            {
                return {fun_, tuple_transform(rngs_, ranges::begin)};
            }
            end_cursor_t<false> end_cursor()
            {
                return {fun_, tuple_transform(rngs_, ranges::end)};
            }
            template(bool Const = true)(
                requires Const AND and_v<range<Rngs const>...> AND
                    views::zippable_with<Fun, meta::if_c<Const, Rngs const>...>)
                cursor<Const> begin_cursor() const
            {
                return {fun_, tuple_transform(rngs_, ranges::begin)};
            }
            template(bool Const = true)(
                requires Const AND and_v<range<Rngs const>...> AND
                    views::zippable_with<Fun, meta::if_c<Const, Rngs const>...>)
                end_cursor_t<Const> end_cursor() const
            {
                return {fun_, tuple_transform(rngs_, ranges::end)};
            }

        public:
            iter_zip_with_view() = default;
            explicit iter_zip_with_view(Rngs... rngs)
              : fun_(Fun{})
              , rngs_{eastl::move(rngs)...}
            {}
            explicit iter_zip_with_view(Fun fun, Rngs... rngs)
              : fun_(eastl::move(fun))
              , rngs_{eastl::move(rngs)...}
            {}
            CPP_auto_member constexpr auto CPP_fun(size)()(
                const //
                requires and_v<sized_range<Rngs const>...>)
            {
                using size_type = common_type_t<range_size_t<Rngs const>...>;
                return range_cardinality<iter_zip_with_view>::value >= 0
                           ? size_type{(
                                 std::size_t)range_cardinality<iter_zip_with_view>::value}
                           : tuple_foldl(tuple_transform(rngs_,
                                                         [](auto && r) -> size_type {
                                                             return ranges::size(r);
                                                         }),
                                         (eastl::numeric_limits<size_type>::max)(),
                                         detail::min_);
            }
        };

        template<typename Fun, typename... Rngs>
        struct zip_with_view : iter_zip_with_view<indirected<Fun>, Rngs...>
        {
            CPP_assert(sizeof...(Rngs) != 0);

            zip_with_view() = default;
            explicit zip_with_view(Rngs... rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{{Fun{}},
                                                             eastl::move(rngs)...}
            {}
            explicit zip_with_view(Fun fun, Rngs... rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{{eastl::move(fun)},
                                                             eastl::move(rngs)...}
            {}
        };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename Fun, typename... Rng)(requires copy_constructible<Fun>)
            zip_with_view(Fun, Rng &&...) -> zip_with_view<Fun, views::all_t<Rng>...>;
#endif

        namespace views
        {
            struct iter_zip_with_fn
            {
                template(typename... Rngs,
                         typename Fun)(requires and_v<viewable_range<Rngs>...> AND
                                           zippable_with<Fun, Rngs...>
                                               AND(sizeof...(Rngs) != 0)) //
                    iter_zip_with_view<Fun, all_t<Rngs>...>               //
                    operator()(Fun fun, Rngs &&... rngs) const
                {
                    return iter_zip_with_view<Fun, all_t<Rngs>...>{
                        eastl::move(fun), all(static_cast<Rngs &&>(rngs))...};
                }

                template(typename Fun)(requires zippable_with<Fun>) //
                    constexpr empty_view<detail::decay_t<invoke_result_t<Fun &>>>
                    operator()(Fun) const noexcept
                {
                    return {};
                }
            };

            /// \relates iter_zip_with_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(iter_zip_with_fn, iter_zip_with)

            struct zip_with_fn
            {
                template(typename... Rngs, typename Fun)(
                    requires and_v<viewable_range<Rngs>...> AND
                        and_v<input_range<Rngs>...>
                            AND copy_constructible<Fun>
                                AND invocable<Fun &, range_reference_t<Rngs>...>
                                    AND(sizeof...(Rngs) != 0)) //
                    zip_with_view<Fun, all_t<Rngs>...>
                    operator()(Fun fun, Rngs &&... rngs) const
                {
                    return zip_with_view<Fun, all_t<Rngs>...>{
                        eastl::move(fun), all(static_cast<Rngs &&>(rngs))...};
                }

                template(typename Fun)(
                    requires copy_constructible<Fun> AND invocable<Fun &>) //
                    constexpr empty_view<detail::decay_t<invoke_result_t<Fun &>>>
                    operator()(Fun) const noexcept
                {
                    return {};
                }
            };

            /// \relates zip_with_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(zip_with_fn, zip_with)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::iter_zip_with_view)
EARANGES_SATISFY_BOOST_RANGE(::ranges::zip_with_view)

#endif
