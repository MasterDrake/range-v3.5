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

#ifndef EARANGES_ACTION_PUSH_BACK_HPP
#define EARANGES_ACTION_PUSH_BACK_HPP

#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/insert.hpp>
#include <EARanges/detail/with_braced_init_args.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-actions
        /// @{

        /// \cond
        namespace adl_push_back_detail
        {
            /// \endcond
            template<typename Cont, typename T>
            using push_back_t =
                decltype(static_cast<void>(unwrap_reference(eastl::declval<Cont &>())
                                               .push_back(eastl::declval<T>())));

            template<typename Cont, typename Rng>
            using insert_t = decltype(static_cast<void>(ranges::insert(
                eastl::declval<Cont &>(), eastl::declval<sentinel_t<Cont>>(),
                eastl::declval<Rng>())));

            template(typename Cont,
                     typename T)(requires lvalue_container_like<Cont> AND(!range<T>)
                                     AND constructible_from<range_value_t<Cont>, T>)
                push_back_t<Cont, T> push_back(Cont && cont, T && t)
            {
                unwrap_reference(cont).push_back(static_cast<T &&>(t));
            }

            template(typename Cont,
                     typename Rng)(requires lvalue_container_like<Cont> AND range<Rng>)
                insert_t<Cont, Rng> push_back(Cont && cont, Rng && rng)
            {
                ranges::insert(cont, end(cont), static_cast<Rng &&>(rng));
            }

            /// \cond
            // clang-format off
        /// \concept can_push_back_frag_
        /// \brief The \c can_push_back_frag_ concept
        template<typename Rng, typename T>
        CPP_requires(can_push_back_frag_,
            requires(Rng && rng, T && t) //
            (
                push_back(rng, (T &&) t)
            ));
        /// \concept can_push_back_
        /// \brief The \c can_push_back_ concept
        template<typename Rng, typename T>
        CPP_concept can_push_back_ =
            CPP_requires_ref(adl_push_back_detail::can_push_back_frag_, Rng, T);
            // clang-format on
            /// \endcond

            struct push_back_fn
            {
                template<typename T>
                constexpr auto operator()(T && val) const
                {
                    return make_action_closure(
                        bind_back(push_back_fn{}, static_cast<T &&>(val)));
                }

                template(typename T)(requires range<T &>) constexpr auto operator()(
                    T & t) const
                {
                    return make_action_closure(
                        bind_back(push_back_fn{}, detail::reference_wrapper_<T>(t)));
                }

                template<typename T>
                constexpr auto operator()(std::initializer_list<T> val) const
                {
                    return make_action_closure(bind_back(push_back_fn{}, val));
                }

                template(typename Rng, typename T)(
                    requires input_range<Rng> AND can_push_back_<Rng, T> AND(
                        range<T> || constructible_from<range_value_t<Rng>, T>)) //
                    Rng
                    operator()(Rng && rng, T && t) const //
                {
                    push_back(rng, static_cast<T &&>(t));
                    return static_cast<Rng &&>(rng);
                }

                template(typename Rng, typename T)(
                    requires input_range<Rng> AND
                        can_push_back_<Rng, std::initializer_list<T>>
                            AND constructible_from<range_value_t<Rng>, T const &>) Rng
                operator()(Rng && rng, std::initializer_list<T> t) const //
                {
                    push_back(rng, t);
                    return static_cast<Rng &&>(rng);
                }

                /// \cond
                template<typename Rng, typename T>
                invoke_result_t<push_back_fn, Rng, T &> //
                operator()(Rng && rng, detail::reference_wrapper_<T> r) const
                {
                    return (*this)(static_cast<Rng &&>(rng), r.get());
                }
                /// \endcond
            };
            /// \cond
        } // namespace adl_push_back_detail
        /// \endcond

        namespace actions
        {
            EARANGES_INLINE_VARIABLE(adl_push_back_detail::push_back_fn, push_back)
        } // namespace actions

        using actions::push_back;

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif