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

#ifndef EARANGES_UTILITY_COMMON_TUPLE_HPP
#define EARANGES_UTILITY_COMMON_TUPLE_HPP


#include <EASTL/tuple.h>
#include <EASTL/utility.h>
#include <EASTL/type_traits.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/detail/adl_get.hpp>
#include <EARanges/functional/bind.hpp>
#include <EARanges/functional/reference_wrapper.hpp>
#include <EARanges/utility/common_type.hpp>
#include <EARanges/utility/tuple_algorithm.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \cond
        namespace detail
        {
            template<typename... Us, typename Tup, std::size_t... Is>
            eastl::tuple<Us...> to_std_tuple(Tup && tup, meta::index_sequence<Is...>)
            {
                return eastl::tuple<Us...>{adl_get<Is>(static_cast<Tup &&>(tup))...};
            }

#ifdef EARANGES_WORKAROUND_MSVC_786312
            template<std::size_t, typename...>
            struct args_;

            template<typename, typename>
            inline constexpr bool argstructible = false;
            template<std::size_t N, typename... Ts, typename... Us>
            inline constexpr bool argstructible<args_<N, Ts...>, args_<N, Us...>> =
                (META_IS_CONSTRUCTIBLE(Ts, Us) && ...);

            template<typename, typename>
            inline constexpr bool argsignable = false;
            template<std::size_t N, typename... Ts, typename... Us>
            inline constexpr bool argsignable<args_<N, Ts...>, args_<N, Us...>> =
                (eastl::is_assignable_v<Ts &, Us> && ...);
#endif // EARANGES_WORKAROUND_MSVC_786312

            template<std::size_t N, typename... Ts>
            struct args_
            {
                template<typename... Us>
                args_(args_<N, Us...>, meta::if_c<
#ifdef EARANGES_WORKAROUND_MSVC_786312
                                           argstructible<args_, args_<N, Us...>>
#else  // ^^^ workaround / no workaround vvv
                                           meta::and_c<
                                               META_IS_CONSTRUCTIBLE(Ts, Us)...>::value
#endif // EARANGES_WORKAROUND_MSVC_786312
                                           > * = nullptr)
                {}
                template<typename... Us>
                meta::if_c<
#ifdef EARANGES_WORKAROUND_MSVC_786312
                    argsignable<args_, args_<N, Us...>>,
#else  // ^^^ workaround / no workaround vvv
                    meta::and_c<eastl::is_assignable<Ts &, Us>::value...>::value,
#endif // EARANGES_WORKAROUND_MSVC_786312
                    args_ &>
                operator=(args_<N, Us...>)
                {
                    return *this;
                }
            };
            template<typename... Ts>
            using args = args_<sizeof...(Ts), Ts...>;
            template<typename... Ts>
            using rargs = args_<sizeof...(Ts), Ts &...>;
        } // namespace detail
        /// \endcond

        template<typename... Ts>
        struct common_tuple
          : _tuple_wrapper_::forward_tuple_interface<eastl::tuple<Ts...>>
        {
        private:
            template<typename That, std::size_t... Is>
            common_tuple(That && that, meta::index_sequence<Is...>)
              : common_tuple::forward_tuple_interface{
                    detail::adl_get<Is>(static_cast<That &&>(that))...}
            {}
            struct element_assign_
            {
                template<typename T, typename U>
                int operator()(T & t, U && u) const
                {
                    t = static_cast<U &&>(u);
                    return 0;
                }
            };

        public:
            // Construction
            CPP_member
            CPP_ctor(common_tuple)()( //
                noexcept(             //
                    meta::and_c<eastl::is_nothrow_default_constructible<Ts>::value...>::
                        value) requires default_constructible<eastl::tuple<Ts...>>)
              : common_tuple::forward_tuple_interface{}
            {}
            template(typename... Us)(
                requires constructible_from<
                    detail::args<Ts...>,
                    detail::args<Us...>>) explicit common_tuple(Us &&... us) //
                noexcept(
                    meta::and_c<eastl::is_nothrow_constructible<Ts, Us>::value...>::value)
              : common_tuple::forward_tuple_interface{static_cast<Us &&>(us)...}
            {}
            template(typename... Us)(
                requires constructible_from<detail::args<Ts...>, detail::rargs<Us...>>)
                common_tuple(eastl::tuple<Us...> & that) //
                noexcept(meta::and_c<
                         eastl::is_nothrow_constructible<Ts, Us &>::value...>::value) //
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            template(typename... Us)(requires constructible_from<
                                     detail::args<Ts...>, detail::rargs<Us const...>>)
                common_tuple(eastl::tuple<Us...> const & that) //
                noexcept(meta::and_c<eastl::is_nothrow_constructible<
                             Ts, Us const &>::value...>::value) //
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            template(typename... Us)(
                requires constructible_from<detail::args<Ts...>, detail::args<Us...>>)
                common_tuple(eastl::tuple<Us...> && that) //
                noexcept(meta::and_c<
                         eastl::is_nothrow_constructible<Ts, Us>::value...>::value) //
              : common_tuple(eastl::move(that),
                             meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            template(typename... Us)(
                requires constructible_from<detail::args<Ts...>, detail::rargs<Us...>>)
                common_tuple(common_tuple<Us...> & that) //
                noexcept(meta::and_c<
                         eastl::is_nothrow_constructible<Ts, Us &>::value...>::value) //
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            template(typename... Us)(requires constructible_from<
                                     detail::args<Ts...>, detail::rargs<Us const...>>)
                common_tuple(common_tuple<Us...> const & that) //
                noexcept(meta::and_c<eastl::is_nothrow_constructible<
                             Ts, Us const &>::value...>::value) //
              : common_tuple(that, meta::make_index_sequence<sizeof...(Ts)>{})
            {}
            template(typename... Us)(
                requires constructible_from<detail::args<Ts...>, detail::args<Us...>>)
                common_tuple(common_tuple<Us...> && that) //
                noexcept(meta::and_c<
                         eastl::is_nothrow_constructible<Ts, Us>::value...>::value) //
              : common_tuple(eastl::move(that),
                             meta::make_index_sequence<sizeof...(Ts)>{})
            {}

            eastl::tuple<Ts...> & base() noexcept
            {
                return *this;
            }
            eastl::tuple<Ts...> const & base() const noexcept
            {
                return *this;
            }

            // Assignment
            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts...> &,
                                              detail::rargs<Us...>>::value) //
                common_tuple &
                operator=(eastl::tuple<Us...> & that) noexcept(
                    meta::and_c<
                        eastl::is_nothrow_assignable<Ts &, Us &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts...> &,
                                              detail::rargs<Us const...>>::value) //
                common_tuple &
                operator=(eastl::tuple<Us...> const & that) noexcept(
                    meta::and_c<
                        eastl::is_nothrow_assignable<Ts &, Us const &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts...> &,
                                              detail::args<Us...>>::value) //
                common_tuple &
                operator=(eastl::tuple<Us...> && that) noexcept(
                    meta::and_c<eastl::is_nothrow_assignable<Ts &, Us>::value...>::value)
            {
                (void)tuple_transform(base(), eastl::move(that), element_assign_{});
                return *this;
            }

            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts const...> &,
                                              detail::rargs<Us...>>::value)
                common_tuple const &
                operator=(eastl::tuple<Us...> & that) const
                noexcept(meta::and_c<
                         eastl::is_nothrow_assignable<Ts const &, Us &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts const...> &,
                                              detail::rargs<Us const...>>::value)
                common_tuple const &
                operator=(eastl::tuple<Us...> const & that) const
                noexcept(meta::and_c<eastl::is_nothrow_assignable<
                             Ts const &, Us const &>::value...>::value)
            {
                (void)tuple_transform(base(), that, element_assign_{});
                return *this;
            }
            template(typename... Us)(
                requires eastl::is_assignable<detail::args<Ts const...> &,
                                              detail::args<Us...>>::value)
                common_tuple const &
                operator=(eastl::tuple<Us...> && that) const noexcept(
                    meta::and_c<
                        eastl::is_nothrow_assignable<Ts const &, Us &&>::value...>::value)
            {
                (void)tuple_transform(base(), eastl::move(that), element_assign_{});
                return *this;
            }

            // Conversion
            template(typename... Us)(
                requires constructible_from<detail::args<Us...>, detail::rargs<Ts...>>)
            operator eastl::tuple<Us...>() & noexcept(
                meta::and_c<eastl::is_nothrow_constructible<Us, Ts &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(
                    *this, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            template(typename... Us)(requires constructible_from<
                                     detail::args<Us...>, detail::rargs<Ts const...>>)
            operator eastl::tuple<Us...>() const & noexcept(
                meta::and_c<
                    eastl::is_nothrow_constructible<Us, Ts const &>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(
                    *this, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            template(typename... Us)(
                requires constructible_from<detail::args<Us...>, detail::args<Ts...>>)
            operator eastl::tuple<Us...>() && noexcept(
                meta::and_c<eastl::is_nothrow_constructible<Us, Ts>::value...>::value)
            {
                return detail::to_std_tuple<Us...>(
                    eastl::move(*this), meta::make_index_sequence<sizeof...(Ts)>{});
            }
        };

        // Logical operators
#define LOGICAL_OP(OP, CONCEPT)                                                       \
    template(typename... Ts, typename... Us)(requires and_v<CONCEPT<Ts, Us>...>) bool \
    operator OP(common_tuple<Ts...> const & a, common_tuple<Us...> const & b)         \
    {                                                                                 \
        return a.base() OP b.base();                                                  \
    }                                                                                 \
    template(typename... Ts, typename... Us)(requires and_v<CONCEPT<Ts, Us>...>) bool \
    operator OP(eastl::tuple<Ts...> const & a, common_tuple<Us...> const & b)         \
    {                                                                                 \
        return a OP b.base();                                                         \
    }                                                                                 \
    template(typename... Ts, typename... Us)(requires and_v<CONCEPT<Ts, Us>...>) bool \
    operator OP(common_tuple<Ts...> const & a, eastl::tuple<Us...> const & b)         \
    {                                                                                 \
        return a.base() OP b;                                                         \
    }                                                                                 \
        /**/
        LOGICAL_OP(==, equality_comparable_with)
        LOGICAL_OP(!=, equality_comparable_with)
        LOGICAL_OP(<, totally_ordered_with)
        LOGICAL_OP(<=, totally_ordered_with)
        LOGICAL_OP(>, totally_ordered_with)
        LOGICAL_OP(>=, totally_ordered_with)
#undef LOGICAL_OP

        struct make_common_tuple_fn
        {
            template<typename... Args>
            common_tuple<bind_element_t<Args>...> operator()(Args &&... args) const
                noexcept(
                    meta::and_c<eastl::is_nothrow_constructible<
                        bind_element_t<Args>, unwrap_reference_t<Args>>::value...>::value)
            {
                return common_tuple<bind_element_t<Args>...>{
                    unwrap_reference(static_cast<Args &&>(args))...};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_tuple_fn`
        EARANGES_INLINE_VARIABLE(make_common_tuple_fn, make_common_tuple)

        template<typename F, typename S>
        struct common_pair : eastl::pair<F, S>
        {
        private:
            eastl::pair<F, S> const & base() const noexcept
            {
                return *this;
            }

        public:
            // Construction
            CPP_member
            CPP_ctor(common_pair)()(                                          //
                noexcept(eastl::is_nothrow_default_constructible<F>::value && //
                         eastl::is_nothrow_default_constructible<S>::value)   //
                requires default_constructible<F> && default_constructible<S>)
              : eastl::pair<F, S>{}
            {}
            template(typename F2, typename S2)(
                requires constructible_from<F, F2> AND constructible_from<S, S2>)
                common_pair(F2 && f2, S2 && s2) //
                noexcept(eastl::is_nothrow_constructible<F, F2>::value &&
                         eastl::is_nothrow_constructible<S, S2>::value) //
              : eastl::pair<F, S>{static_cast<F2 &&>(f2), static_cast<S2 &&>(s2)}
            {}
            template(typename F2, typename S2)(
                requires constructible_from<F, F2 &> AND constructible_from<S, S2 &>)
                common_pair(eastl::pair<F2, S2> & that) //
                noexcept(eastl::is_nothrow_constructible<F, F2 &>::value &&
                         eastl::is_nothrow_constructible<S, S2 &>::value) //
              : eastl::pair<F, S>{that.first, that.second}
            {}
            template(typename F2,
                     typename S2)(requires constructible_from<F, F2 const &> AND
                                      constructible_from<S, S2 const &>)
                common_pair(eastl::pair<F2, S2> const & that) //
                noexcept(eastl::is_nothrow_constructible<F, F2 const &>::value &&
                         eastl::is_nothrow_constructible<S, S2 const &>::value) //
              : eastl::pair<F, S>{that.first, that.second}
            {}
            template(typename F2, typename S2)(
                requires constructible_from<F, F2> AND constructible_from<S, S2>)
                common_pair(eastl::pair<F2, S2> && that) //
                noexcept(eastl::is_nothrow_constructible<F, F2>::value &&
                         eastl::is_nothrow_constructible<S, S2>::value) //
              : eastl::pair<F, S>{eastl::forward<F2>(that.first),
                                  eastl::forward<S2>(that.second)}
            {}

            // Conversion
            template(typename F2, typename S2)(
                requires constructible_from<F2, F &> AND constructible_from<S2, S &>)
            operator eastl::pair<F2, S2>() & //
                noexcept(eastl::is_nothrow_constructible<F2, F &>::value &&
                         eastl::is_nothrow_constructible<S2, S &>::value)
            {
                return {this->first, this->second};
            }
            template(typename F2,
                     typename S2)(requires constructible_from<F2, F const &> AND
                                      constructible_from<S2, S const &>)
            operator eastl::pair<F2, S2>() const & //
                noexcept(eastl::is_nothrow_constructible<F2, F const &>::value &&
                         eastl::is_nothrow_constructible<S2, S const &>::value)
            {
                return {this->first, this->second};
            }
            template(typename F2, typename S2)(
                requires constructible_from<F2, F> AND constructible_from<S2, S>)
            operator eastl::pair<F2, S2>() && noexcept(
                eastl::is_nothrow_constructible<F2, F>::value &&
                eastl::is_nothrow_constructible<S2, S>::value)
            {
                return {eastl::forward<F>(this->first), eastl::forward<S>(this->second)};
            }

            // Assignment
            template(typename F2,
                     typename S2)(requires assignable_from<F &, F2 &> AND
                                      assignable_from<S &, S2 &>) common_pair &
            operator=(eastl::pair<F2, S2> & that) //
                noexcept(eastl::is_nothrow_assignable<F &, F2 &>::value &&
                         eastl::is_nothrow_assignable<S &, S2 &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template(typename F2,
                     typename S2)(requires assignable_from<F &, F2 const &> AND
                                      assignable_from<S &, S2 const &>) common_pair &
            operator=(eastl::pair<F2, S2> const & that) //
                noexcept(eastl::is_nothrow_assignable<F &, F2 const &>::value &&
                         eastl::is_nothrow_assignable<S &, S2 const &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template(typename F2, typename S2)(requires assignable_from<F &, F2> AND
                                                   assignable_from<S &, S2>) common_pair &
            operator=(eastl::pair<F2, S2> && that) //
                noexcept(eastl::is_nothrow_assignable<F &, F2>::value &&
                         eastl::is_nothrow_assignable<S &, S2>::value)
            {
                this->first = static_cast<F2 &&>(that.first);
                this->second = static_cast<S2 &&>(that.second);
                return *this;
            }

            template(typename F2, typename S2)(
                requires assignable_from<F const &, F2 &> AND
                    assignable_from<S const &, S2 &>) common_pair const &
            operator=(eastl::pair<F2, S2> & that) const //
                noexcept(eastl::is_nothrow_assignable<F const &, F2 &>::value &&
                         eastl::is_nothrow_assignable<S const &, S2 &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template(typename F2, typename S2)(
                requires assignable_from<F const &, F2 const &> AND
                    assignable_from<S const &, S2 const &>) common_pair const &
            operator=(eastl::pair<F2, S2> const & that) const //
                noexcept(eastl::is_nothrow_assignable<F const &, F2 const &>::value &&
                         eastl::is_nothrow_assignable<S const &, S2 const &>::value)
            {
                this->first = that.first;
                this->second = that.second;
                return *this;
            }
            template(typename F2,
                     typename S2)(requires assignable_from<F const &, F2> AND
                                      assignable_from<S const &, S2>) common_pair const &
            operator=(eastl::pair<F2, S2> && that) const //
                noexcept(eastl::is_nothrow_assignable<F const &, F2 &&>::value &&
                         eastl::is_nothrow_assignable<S const &, S2 &&>::value)
            {
                this->first = static_cast<F2 &&>(that.first);
                this->second = static_cast<S2 &&>(that.second);
                return *this;
            }
        };

        // Logical operators
        template(typename F1, typename S1, typename F2,
                 typename S2)(requires equality_comparable_with<F1, F2> AND
                                  equality_comparable_with<S1, S2>) bool
        operator==(common_pair<F1, S1> const & a, common_pair<F2, S2> const & b)
        {
            return a.first == b.first && a.second == b.second;
        }
        template(typename F1, typename S1, typename F2,
                 typename S2)(requires equality_comparable_with<F1, F2> AND
                                  equality_comparable_with<S1, S2>) bool
        operator==(common_pair<F1, S1> const & a, eastl::pair<F2, S2> const & b)
        {
            return a.first == b.first && a.second == b.second;
        }
        template(typename F1, typename S1, typename F2,
                 typename S2)(requires equality_comparable_with<F1, F2> AND
                                  equality_comparable_with<S1, S2>) bool
        operator==(eastl::pair<F1, S1> const & a, common_pair<F2, S2> const & b)
        {
            return a.first == b.first && a.second == b.second;
        }
        template(typename F1, typename S1, typename F2, typename S2)(
            requires totally_ordered_with<F1, F2> AND totally_ordered_with<S1, S2>) bool
        operator<(common_pair<F1, S1> const & a, common_pair<F2, S2> const & b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
        template(typename F1, typename S1, typename F2, typename S2)(
            requires totally_ordered_with<F1, F2> AND totally_ordered_with<S1, S2>) bool
        operator<(eastl::pair<F1, S1> const & a, common_pair<F2, S2> const & b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
        template(typename F1, typename S1, typename F2, typename S2)(
            requires totally_ordered_with<F1, F2> AND totally_ordered_with<S1, S2>) bool
        operator<(common_pair<F1, S1> const & a, eastl::pair<F2, S2> const & b)
        {
            return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
        }
#define LOGICAL_OP(OP, CONCEPT, RET)                                          \
    template(typename F1, typename S1, typename F2, typename S2)(             \
        requires CONCEPT<F1, F2> AND CONCEPT<S1, S2>) bool                    \
    operator OP(common_pair<F1, S1> const & a, common_pair<F2, S2> const & b) \
    {                                                                         \
        return RET;                                                           \
    }                                                                         \
    template(typename F1, typename S1, typename F2, typename S2)(             \
        requires CONCEPT<F1, F2> AND CONCEPT<S1, S2>) bool                    \
    operator OP(eastl::pair<F1, S1> const & a, common_pair<F2, S2> const & b) \
    {                                                                         \
        return RET;                                                           \
    }                                                                         \
    template(typename F1, typename S1, typename F2, typename S2)(             \
        requires CONCEPT<F1, F2> AND CONCEPT<S1, S2>) bool                    \
    operator OP(common_pair<F1, S1> const & a, eastl::pair<F2, S2> const & b) \
    {                                                                         \
        return RET;                                                           \
    }                                                                         \
        /**/
        LOGICAL_OP(!=, equality_comparable_with, !(a == b))
        LOGICAL_OP(<=, totally_ordered_with, !(b < a))
        LOGICAL_OP(>, totally_ordered_with, (b < a))
        LOGICAL_OP(>=, totally_ordered_with, !(a < b))
#undef LOGICAL_OP

        struct make_common_pair_fn
        {
            template<typename First, typename Second, typename F = bind_element_t<First>,
                     typename S = bind_element_t<Second>>
            common_pair<F, S> operator()(First && f, Second && s) const //
                noexcept(
                    eastl::is_nothrow_constructible<F,
                                                    unwrap_reference_t<First>>::value &&
                    eastl::is_nothrow_constructible<F, unwrap_reference_t<Second>>::value)
            {
                return {unwrap_reference(static_cast<First &&>(f)),
                        unwrap_reference(static_cast<Second &&>(s))};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_common_pair_fn`
        EARANGES_INLINE_VARIABLE(make_common_pair_fn, make_common_pair)

        /// \cond
        namespace detail
        {
            template<typename, typename, typename, typename = void>
            struct common_type_tuple_like
            {};

            template<template<typename...> class T0, typename... Ts,
                     template<typename...> class T1, typename... Us, typename TupleLike>
            struct common_type_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
                                          meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
              : meta::lazy::let<
                    meta::lazy::invoke<TupleLike, meta::lazy::_t<common_type<Ts, Us>>...>>
            {};

            template<typename, typename, typename, typename = void>
            struct common_ref_tuple_like
            {};

            template<template<typename...> class T0, typename... Ts,
                     template<typename...> class T1, typename... Us, typename TupleLike>
            struct common_ref_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
                                         meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
              : meta::lazy::let<meta::lazy::invoke<
                    TupleLike, meta::lazy::_t<common_reference<Ts, Us>>...>>
            {};
        } // namespace detail
        /// \endcond
    } // namespace ranges

    /// \cond
    namespace concepts
    {
        // common_type for pairs
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<eastl::pair<F1, S1>, ranges::common_pair<F2, S2>>
          : ranges::detail::common_type_tuple_like<eastl::pair<F1, S1>,
                                                   ranges::common_pair<F2, S2>,
                                                   meta::quote<ranges::common_pair>>
        {};
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<ranges::common_pair<F1, S1>, eastl::pair<F2, S2>>
          : ranges::detail::common_type_tuple_like<ranges::common_pair<F1, S1>,
                                                   eastl::pair<F2, S2>,
                                                   meta::quote<ranges::common_pair>>
        {};
        template<typename F1, typename S1, typename F2, typename S2>
        struct common_type<ranges::common_pair<F1, S1>, ranges::common_pair<F2, S2>>
          : ranges::detail::common_type_tuple_like<ranges::common_pair<F1, S1>,
                                                   ranges::common_pair<F2, S2>,
                                                   meta::quote<ranges::common_pair>>
        {};
        // common_type for tuples
        template<typename... Ts, typename... Us>
        struct common_type<ranges::common_tuple<Ts...>, eastl::tuple<Us...>>
          : ranges::detail::common_type_tuple_like<ranges::common_tuple<Ts...>,
                                                   eastl::tuple<Us...>,
                                                   meta::quote<ranges::common_tuple>>
        {};
        template<typename... Ts, typename... Us>
        struct common_type<eastl::tuple<Ts...>, ranges::common_tuple<Us...>>
          : ranges::detail::common_type_tuple_like<eastl::tuple<Ts...>,
                                                   ranges::common_tuple<Us...>,
                                                   meta::quote<ranges::common_tuple>>
        {};
        template<typename... Ts, typename... Us>
        struct common_type<ranges::common_tuple<Ts...>, ranges::common_tuple<Us...>>
          : ranges::detail::common_type_tuple_like<ranges::common_tuple<Ts...>,
                                                   ranges::common_tuple<Us...>,
                                                   meta::quote<ranges::common_tuple>>
        {};

        // common reference for pairs
        template<typename F1, typename S1, typename F2, typename S2,
                 template<typename> class Qual1, template<typename> class Qual2>
        struct basic_common_reference<ranges::common_pair<F1, S1>, eastl::pair<F2, S2>,
                                      Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<
                ranges::common_pair<Qual1<F1>, Qual1<S1>>,
                eastl::pair<Qual2<F2>, Qual2<S2>>, meta::quote<ranges::common_pair>>
        {};
        template<typename F1, typename S1, typename F2, typename S2,
                 template<typename> class Qual1, template<typename> class Qual2>
        struct basic_common_reference<eastl::pair<F1, S1>, ranges::common_pair<F2, S2>,
                                      Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<
                eastl::pair<Qual1<F1>, Qual1<S1>>,
                ranges::common_pair<Qual2<F2>, Qual2<S2>>,
                meta::quote<ranges::common_pair>>
        {};
        template<typename F1, typename S1, typename F2, typename S2,
                 template<typename> class Qual1, template<typename> class Qual2>
        struct basic_common_reference<ranges::common_pair<F1, S1>,
                                      ranges::common_pair<F2, S2>, Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<
                ranges::common_pair<Qual1<F1>, Qual1<S1>>,
                ranges::common_pair<Qual2<F2>, Qual2<S2>>,
                meta::quote<ranges::common_pair>>
        {};
        // common reference for tuples
        template<typename... Ts, typename... Us, template<typename> class Qual1,
                 template<typename> class Qual2>
        struct basic_common_reference<ranges::common_tuple<Ts...>, eastl::tuple<Us...>,
                                      Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<ranges::common_tuple<Qual1<Ts>...>,
                                                  eastl::tuple<Qual2<Us>...>,
                                                  meta::quote<ranges::common_tuple>>
        {};
        template<typename... Ts, typename... Us, template<typename> class Qual1,
                 template<typename> class Qual2>
        struct basic_common_reference<eastl::tuple<Ts...>, ranges::common_tuple<Us...>,
                                      Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<eastl::tuple<Qual1<Ts>...>,
                                                  ranges::common_tuple<Qual2<Us>...>,
                                                  meta::quote<ranges::common_tuple>>
        {};
        template<typename... Ts, typename... Us, template<typename> class Qual1,
                 template<typename> class Qual2>
        struct basic_common_reference<ranges::common_tuple<Ts...>,
                                      ranges::common_tuple<Us...>, Qual1, Qual2>
          : ranges::detail::common_ref_tuple_like<ranges::common_tuple<Qual1<Ts>...>,
                                                  ranges::common_tuple<Qual2<Us>...>,
                                                  meta::quote<ranges::common_tuple>>
        {};
    } // namespace concepts
    /// \endcond

    EARANGES_DIAGNOSTIC_PUSH
    EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
    EARANGES_BEGIN_NAMESPACE_STD
        template<typename First, typename Second>
        struct tuple_size<::ranges::common_pair<First, Second>>
          : eastl::integral_constant<size_t, 2>
        {};

        template<typename First, typename Second>
        struct tuple_element<0, ::ranges::common_pair<First, Second>>
        {
            using type = First;
        };

        template<typename First, typename Second>
        struct tuple_element<1, ::ranges::common_pair<First, Second>>
        {
            using type = Second;
        };

        template<typename... Ts>
        struct tuple_size<::ranges::common_tuple<Ts...>>
          : eastl::integral_constant<size_t, sizeof...(Ts)>
        {};

        template<size_t N, typename... Ts>
        struct tuple_element<N, ::ranges::common_tuple<Ts...>>
          : tuple_element<N, tuple<Ts...>>
        {};

#if EARANGES_CXX_VER > EARANGES_CXX_STD_17
        EARANGES_BEGIN_NAMESPACE_VERSION
            template<typename...>
            struct common_type;

            // common_type for pairs
            template<typename F1, typename S1, typename F2, typename S2>
            struct common_type<eastl::pair<F1, S1>, ::ranges::common_pair<F2, S2>>
              : ::ranges::detail::common_type_tuple_like<
                    eastl::pair<F1, S1>,
                    ::ranges::common_pair<F2, S2>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            template<typename F1, typename S1, typename F2, typename S2>
            struct common_type<::ranges::common_pair<F1, S1>, eastl::pair<F2, S2>>
              : ::ranges::detail::common_type_tuple_like<
                    ::ranges::common_pair<F1, S1>,
                    eastl::pair<F2, S2>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            template<typename F1, typename S1, typename F2, typename S2>
            struct common_type<::ranges::common_pair<F1, S1>,
                               ::ranges::common_pair<F2, S2>>
              : ::ranges::detail::common_type_tuple_like<
                    ::ranges::common_pair<F1, S1>,
                    ::ranges::common_pair<F2, S2>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            // common_type for tuples
            template<typename... Ts, typename... Us>
            struct common_type<::ranges::common_tuple<Ts...>, eastl::tuple<Us...>>
              : ::ranges::detail::common_type_tuple_like<
                    ::ranges::common_tuple<Ts...>,
                    eastl::tuple<Us...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};
            template<typename... Ts, typename... Us>
            struct common_type<eastl::tuple<Ts...>, ::ranges::common_tuple<Us...>>
              : ::ranges::detail::common_type_tuple_like<
                    eastl::tuple<Ts...>,
                    ::ranges::common_tuple<Us...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};
            template<typename... Ts, typename... Us>
            struct common_type<::ranges::common_tuple<Ts...>,
                               ::ranges::common_tuple<Us...>>
              : ::ranges::detail::common_type_tuple_like<
                    ::ranges::common_tuple<Ts...>,
                    ::ranges::common_tuple<Us...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};

            template<typename,
                     typename,
                     template<typename>
                     class,
                     template<typename>
                     class>
            struct basic_common_reference;

            // common reference for pairs
            template<typename F1,
                     typename S1,
                     typename F2,
                     typename S2,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<::ranges::common_pair<F1, S1>,
                                          eastl::pair<F2, S2>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    ::ranges::common_pair<Qual1<F1>, Qual1<S1>>,
                    eastl::pair<Qual2<F2>, Qual2<S2>>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            template<typename F1,
                     typename S1,
                     typename F2,
                     typename S2,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<eastl::pair<F1, S1>,
                                          ::ranges::common_pair<F2, S2>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    eastl::pair<Qual1<F1>, Qual1<S1>>,
                    ::ranges::common_pair<Qual2<F2>, Qual2<S2>>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            template<typename F1,
                     typename S1,
                     typename F2,
                     typename S2,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<::ranges::common_pair<F1, S1>,
                                          ::ranges::common_pair<F2, S2>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    ::ranges::common_pair<Qual1<F1>, Qual1<S1>>,
                    ::ranges::common_pair<Qual2<F2>, Qual2<S2>>,
                    ::meta::quote<::ranges::common_pair>>
            {};
            // common reference for tuples
            template<typename... Ts,
                     typename... Us,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<::ranges::common_tuple<Ts...>,
                                          eastl::tuple<Us...>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    ::ranges::common_tuple<Qual1<Ts>...>,
                    eastl::tuple<Qual2<Us>...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};
            template<typename... Ts,
                     typename... Us,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<eastl::tuple<Ts...>,
                                          ::ranges::common_tuple<Us...>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    eastl::tuple<Qual1<Ts>...>,
                    ::ranges::common_tuple<Qual2<Us>...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};
            template<typename... Ts,
                     typename... Us,
                     template<typename>
                     class Qual1,
                     template<typename>
                     class Qual2>
            struct basic_common_reference<::ranges::common_tuple<Ts...>,
                                          ::ranges::common_tuple<Us...>,
                                          Qual1,
                                          Qual2>
              : ::ranges::detail::common_ref_tuple_like<
                    ::ranges::common_tuple<Qual1<Ts>...>,
                    ::ranges::common_tuple<Qual2<Us>...>,
                    ::meta::quote<::ranges::common_tuple>>
            {};
        EARANGES_END_NAMESPACE_VERSION
#endif // EARANGES_CXX_VER > EARANGES_CXX_STD_17

    EARANGES_END_NAMESPACE_STD
    EARANGES_DIAGNOSTIC_POP
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
