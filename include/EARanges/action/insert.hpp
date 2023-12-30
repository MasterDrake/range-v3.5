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

#ifndef EARANGES_ACTION_INSERT_HPP
#define EARANGES_ACTION_INSERT_HPP

#include <EASTL/initializer_list.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/concepts.hpp>
#include <EARanges/algorithm/max.hpp>
#include <EARanges/iterator/common_iterator.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace adl_insert_detail
    {
        template<typename Cont, typename... Args>
        using insert_result_t = decltype(unwrap_reference(eastl::declval<Cont>()).insert(eastl::declval<Args>()...));

        template(typename Cont, typename T)(requires lvalue_container_like<Cont> AND (!range<T> && constructible_from<range_value_t<Cont>, T>)) //
        insert_result_t<Cont &, T> insert(Cont && cont, T && t)
        {
            return unwrap_reference(cont).insert(static_cast<T &&>(t));
        }

        template(typename Cont, typename I, typename S)(requires lvalue_container_like<Cont> AND sentinel_for<S, I> AND (!range<S>))
        insert_result_t<Cont &, detail::cpp17_iterator_t<I, S>, detail::cpp17_iterator_t<I, S>>
        insert(Cont && cont, I i, S j)
        {
            return unwrap_reference(cont).insert(detail::cpp17_iterator_t<I, S>{i}, detail::cpp17_iterator_t<I, S>{j});
        }

        template(typename Cont, typename Rng)(requires lvalue_container_like<Cont> AND range<Rng>)
        insert_result_t<Cont &, detail::range_cpp17_iterator_t<Rng>,detail::range_cpp17_iterator_t<Rng>>
        insert(Cont && cont, Rng && rng)
        {
            return unwrap_reference(cont).insert( detail::range_cpp17_iterator_t<Rng>{ranges::begin(rng)}, detail::range_cpp17_iterator_t<Rng>{ranges::end(rng)});
        }

        template(typename Cont, typename I, typename T)(requires lvalue_container_like<Cont> AND input_iterator<I> AND (!range<T> && constructible_from<range_value_t<Cont>, T>)) //
        insert_result_t<Cont &, I, T> insert(Cont && cont, I p, T && t)
        {
            return unwrap_reference(cont).insert(p, static_cast<T &&>(t));
        }

        template(typename Cont, typename I, typename N, typename T)(requires lvalue_container_like<Cont> AND input_iterator<I> AND integral<N> AND constructible_from<range_value_t<Cont>, T>)
        insert_result_t<Cont &, I, N, T> insert(Cont && cont, I p, N n, T && t)
        {
            return unwrap_reference(cont).insert(p, n, static_cast<T &&>(t));
        }

        /// \cond
        namespace detail
        {
            using ranges::detail::cpp17_iterator_t;
            using ranges::detail::range_cpp17_iterator_t;

            template(typename Cont, typename P)(requires container<Cont> AND input_iterator<P> AND random_access_reservable<Cont>)
            iterator_t<Cont> insert_reserve_helper(Cont & cont, P const p, range_size_t<Cont> const delta)
            {
                auto const old_size = ranges::size(cont);
                auto const max_size = cont.max_size();
                EARANGES_EXPECT(delta <= max_size - old_size);
                auto const new_size = old_size + delta;
                auto const old_capacity = cont.capacity();
                auto const index = p - ranges::begin(cont);
                if(old_capacity < new_size)
                {
                    auto const new_capacity =
                        (old_capacity <= max_size / 3 * 2)
                            ? ranges::max(old_capacity + old_capacity / 2, new_size)
                            : max_size;
                    cont.reserve(new_capacity);
                }
                return ranges::begin(cont) + index;
            }

            template(typename Cont, typename P, typename I, typename S)(
                requires sentinel_for<S, I> AND (!range<S>)) //
            auto insert_impl(Cont && cont, P p, I i, S j, eastl::false_type)
                -> decltype(unwrap_reference(cont).insert(
                    p, cpp17_iterator_t<I, S>{i}, cpp17_iterator_t<I, S>{j}))
            {
                using C = cpp17_iterator_t<I, S>;
                return unwrap_reference(cont).insert(p, C{i}, C{j});
            }

            template(typename Cont, typename P, typename I, typename S)(
                requires sized_sentinel_for<S, I> AND random_access_reservable<Cont> AND
                    (!range<S>)) //
            auto insert_impl(Cont && cont_, P p, I i, S j, eastl::true_type)
                -> decltype(unwrap_reference(cont_).insert(
                    ranges::begin(unwrap_reference(cont_)), cpp17_iterator_t<I, S>{i},
                    cpp17_iterator_t<I, S>{j}))
            {
                using C = cpp17_iterator_t<I, S>;
                auto && cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(j - i);
                auto pos = insert_reserve_helper(cont, eastl::move(p), delta);
                return cont.insert(pos, C{eastl::move(i)}, C{eastl::move(j)});
            }

            template(typename Cont, typename I, typename Rng)(
                requires range<Rng>)
            auto insert_impl(Cont && cont, I p, Rng && rng, eastl::false_type)
                -> decltype(unwrap_reference(cont).insert(
                    p, range_cpp17_iterator_t<Rng>{ranges::begin(rng)},
                    range_cpp17_iterator_t<Rng>{ranges::end(rng)}))
            {
                using C = range_cpp17_iterator_t<Rng>;
                return unwrap_reference(cont).insert(
                    p, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }

            template(typename Cont, typename I, typename Rng)(
                requires random_access_reservable<Cont> AND sized_range<Rng>)
            auto insert_impl(Cont && cont_, I p, Rng && rng, eastl::true_type)
                -> decltype(unwrap_reference(cont_).insert(
                    begin(unwrap_reference(cont_)),
                    range_cpp17_iterator_t<Rng>{ranges::begin(rng)},
                    range_cpp17_iterator_t<Rng>{ranges::end(rng)}))
            {
                using C = range_cpp17_iterator_t<Rng>;
                auto && cont = unwrap_reference(cont_);
                auto const delta = static_cast<range_size_t<Cont>>(ranges::size(rng));
                auto pos = insert_reserve_helper(cont, eastl::move(p), delta);
                return cont.insert(pos, C{ranges::begin(rng)}, C{ranges::end(rng)});
            }
        } // namespace detail
        /// \endcond

        template(typename Cont, typename P, typename I, typename S)(
            requires lvalue_container_like<Cont> AND input_iterator<P> AND
                sentinel_for<S, I> AND
            (!range<S>)) //
        auto insert(Cont && cont, P p, I i, S j) //
            -> decltype(detail::insert_impl(
                static_cast<Cont &&>(cont),
                static_cast<P &&>(p),
                static_cast<I &&>(i),
                static_cast<S &&>(j),
                meta::bool_<random_access_reservable<Cont> && //
                            sized_sentinel_for<S, I>>{}))
        {
            return detail::insert_impl(static_cast<Cont &&>(cont),
                                       static_cast<P &&>(p),
                                       static_cast<I &&>(i),
                                       static_cast<S &&>(j),
                                       meta::bool_<random_access_reservable<Cont> &&
                                                   sized_sentinel_for<S, I>>{});
        }

        template(typename Cont, typename I, typename Rng)(
            requires lvalue_container_like<Cont> AND input_iterator<I> AND range<Rng>)
        auto insert(Cont && cont, I p, Rng && rng)
            -> decltype(detail::insert_impl(
                static_cast<Cont &&>(cont), eastl::move(p), static_cast<Rng &&>(rng),
                meta::bool_<random_access_reservable<Cont> && sized_range<Rng>>{}))
        {
            return detail::insert_impl(static_cast<Cont &&>(cont),
                                       eastl::move(p),
                                       static_cast<Rng &&>(rng),
                                       meta::bool_<random_access_reservable<Cont> &&
                                                   sized_range<Rng>>{});
        }

        struct insert_fn
        {
            template<typename Rng, typename... Args>
            using insert_result_t =
                decltype(insert(eastl::declval<Rng>(), eastl::declval<Args>()...));

            template(typename Rng, typename T)(
                requires range<Rng> AND
                    (!range<T>) AND constructible_from<range_value_t<Rng>, T>)
            insert_result_t<Rng, T> operator()(Rng && rng, T && t) const
            {
                return insert(static_cast<Rng &&>(rng), static_cast<T &&>(t));
            }

            template(typename Rng, typename Rng2)(
                requires range<Rng> AND range<Rng2>)
            insert_result_t<Rng, Rng2> operator()(Rng && rng, Rng2 && rng2) const
            {
                static_assert(!is_infinite<Rng>::value, "Attempting to insert an infinite range into a container");
                return insert(static_cast<Rng &&>(rng), static_cast<Rng2 &&>(rng2));
            }

            template(typename Rng, typename T)(requires range<Rng>)
            insert_result_t<Rng, std::initializer_list<T> &> //
            operator()(Rng && rng, std::initializer_list<T> rng2) const
            {
                return insert(static_cast<Rng &&>(rng), rng2);
            }

            template(typename Rng, typename I, typename S)(requires range<Rng> AND sentinel_for<S, I> AND (!range<S>)) //
            insert_result_t<Rng, I, S> operator()(Rng && rng, I i, S j) const
            {
                return insert(static_cast<Rng &&>(rng), eastl::move(i), eastl::move(j));
            }

            template(typename Rng, typename I, typename T)(
                requires range<Rng> AND input_iterator<I> AND
                    (!range<T>) AND constructible_from<range_value_t<Rng>, T>)
            insert_result_t<Rng, I, T> operator()(Rng && rng, I p, T && t) const
            {
                return insert(
                    static_cast<Rng &&>(rng), eastl::move(p), static_cast<T &&>(t));
            }

            template(typename Rng, typename I, typename Rng2)(
                requires range<Rng> AND input_iterator<I> AND range<Rng2>)
            insert_result_t<Rng, I, Rng2> operator()(Rng && rng, I p, Rng2 && rng2) const
            {
                static_assert(!is_infinite<Rng>::value, "Attempting to insert an infinite range into a container");
                return insert(static_cast<Rng &&>(rng), eastl::move(p), static_cast<Rng2 &&>(rng2));
            }

            template(typename Rng, typename I, typename T)(requires range<Rng> AND input_iterator<I>)
            insert_result_t<Rng, I, std::initializer_list<T> &> //
            operator()(Rng && rng, I p, std::initializer_list<T> rng2) const
            {
                return insert(static_cast<Rng &&>(rng), eastl::move(p), rng2);
            }

            template(typename Rng, typename I, typename N, typename T)(
                requires range<Rng> AND input_iterator<I> AND integral<N> AND
                    (!range<T>) AND constructible_from<range_value_t<Rng>, T>)
            insert_result_t<Rng, I, N, T> operator()(Rng && rng, I p, N n, T && t) const
            {
                return insert(
                    static_cast<Rng &&>(rng), eastl::move(p), n, static_cast<T &&>(t));
            }

            template(typename Rng, typename P, typename I, typename S)(requires range<Rng> AND input_iterator<P> AND sentinel_for<S, I> AND (!range<S>)) //
            insert_result_t<Rng, P, I, S> operator()(Rng && rng, P p, I i, S j) const
            {
                return insert(static_cast<Rng &&>(rng), eastl::move(p), eastl::move(i), eastl::move(j));
            }
        };
    } // namespace adl_insert_detail
    /// \endcond

    /// \ingroup group-actions
    EARANGES_INLINE_VARIABLE(adl_insert_detail::insert_fn, insert)

    namespace actions
    {
        using ranges::insert;
    }
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif