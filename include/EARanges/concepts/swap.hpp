/// \file
// Concepts library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef CPP_SWAP_HPP
#define CPP_SWAP_HPP

#include <EASTL/tuple.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

// Note: constexpr implies inline, to retain the same visibility
// C++14 constexpr functions are inline in C++11
#if (defined(__cpp_constexpr) && __cpp_constexpr >= 201304L) ||\
    (!defined(__cpp_constexpr) && __cplusplus >= 201402L)
#define CPP_CXX14_CONSTEXPR constexpr
#else
#define CPP_CXX14_CONSTEXPR inline
#endif

#ifndef CPP_CXX_INLINE_VARIABLES
#ifdef __cpp_inline_variables // TODO: fix this if SD-6 picks another name - Eric Niebler
#define CPP_CXX_INLINE_VARIABLES __cpp_inline_variables
// TODO: remove once clang defines __cpp_inline_variables (or equivalent) - Eric Niebler
#elif defined(__clang__) && \
    (__clang_major__ > 3 || __clang_major__ == 3 && __clang_minor__ == 9) && \
    __cplusplus > 201402L
#define CPP_CXX_INLINE_VARIABLES 201606L
#else
#define CPP_CXX_INLINE_VARIABLES __cplusplus
#endif  // __cpp_inline_variables
#endif  // CPP_CXX_INLINE_VARIABLES

#if defined(_MSC_VER) && !defined(__clang__)
#if _MSC_VER < 1926
#define CPP_WORKAROUND_MSVC_895622 // Error when phase 1 name binding finds only deleted function
#endif // _MSC_VER < 1926
#endif // MSVC

#if CPP_CXX_INLINE_VARIABLES < 201606L
#define CPP_INLINE_VAR
#define CPP_INLINE_VARIABLE(type, name)                                         \
    inline namespace                                                            \
    {                                                                           \
        constexpr auto &name = ::concepts::detail::static_const<type>::value;   \
    }                                                                           \
    /**/
#else  // CPP_CXX_INLINE_VARIABLES >= 201606L
#define CPP_INLINE_VAR inline
#define CPP_INLINE_VARIABLE(type, name)                                         \
    inline constexpr type name{};                                               \
    /**/
#endif // CPP_CXX_INLINE_VARIABLES

#if CPP_CXX_INLINE_VARIABLES < 201606L
#define CPP_DEFINE_CPO(type, name)                                              \
    inline namespace                                                            \
    {                                                                           \
        constexpr auto &name = ::concepts::detail::static_const<type>::value;   \
    }                                                                           \
    /**/
#else  // CPP_CXX_INLINE_VARIABLES >= 201606L
#define CPP_DEFINE_CPO(type, name)                                              \
    inline namespace _                                                          \
    {                                                                           \
        inline constexpr type name{};                                           \
    }                                                                           \
    /**/
#endif // CPP_CXX_INLINE_VARIABLES

#if defined(_MSC_VER) && !defined(__clang__)
#define CPP_DIAGNOSTIC_PUSH __pragma(warning(push))
#define CPP_DIAGNOSTIC_POP __pragma(warning(pop))
#define CPP_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
#define CPP_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define CPP_DIAGNOSTIC_IGNORE_CPP2A_COMPAT
#else // ^^^ defined(_MSC_VER) ^^^ / vvv !defined(_MSC_VER) vvv
#if defined(__GNUC__) || defined(__clang__)
#define CPP_PRAGMA(X) _Pragma(#X)
#define CPP_DIAGNOSTIC_PUSH CPP_PRAGMA(GCC diagnostic push)
#define CPP_DIAGNOSTIC_POP CPP_PRAGMA(GCC diagnostic pop)
#define CPP_DIAGNOSTIC_IGNORE_PRAGMAS \
    CPP_PRAGMA(GCC diagnostic ignored "-Wpragmas")
#define CPP_DIAGNOSTIC_IGNORE(X) \
    CPP_DIAGNOSTIC_IGNORE_PRAGMAS \
    CPP_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
    CPP_PRAGMA(GCC diagnostic ignored X)
#define CPP_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME \
    CPP_DIAGNOSTIC_IGNORE("-Wunknown-warning-option") \
    CPP_DIAGNOSTIC_IGNORE("-Winit-list-lifetime")
#define CPP_DIAGNOSTIC_IGNORE_FLOAT_EQUAL CPP_DIAGNOSTIC_IGNORE("-Wfloat-equal")
#define CPP_DIAGNOSTIC_IGNORE_CPP2A_COMPAT CPP_DIAGNOSTIC_IGNORE("-Wc++2a-compat")
#else
#define CPP_DIAGNOSTIC_PUSH
#define CPP_DIAGNOSTIC_POP
#define CPP_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
#define CPP_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
#define CPP_DIAGNOSTIC_IGNORE_CPP2A_COMPAT
#endif
#endif // MSVC/Generic configuration switch

namespace concepts
{
    /// \cond
    namespace detail
    {
        template<typename T>
        CPP_INLINE_VAR constexpr bool is_movable_v =
            eastl::is_object<T>::value &&
            eastl::is_move_constructible<T>::value &&
            eastl::is_move_assignable<T>::value;

        template<typename T>
        struct static_const
        {
            static constexpr T const value {};
        };
        template<typename T>
        constexpr T const static_const<T>::value;
    }
    /// \endcond

    template<typename T>
    struct is_swappable;

    template<typename T>
    struct is_nothrow_swappable;

    template<typename T, typename U>
    struct is_swappable_with;

    template<typename T, typename U>
    struct is_nothrow_swappable_with;

    template<typename T, typename U = T>
    CPP_CXX14_CONSTEXPR
    meta::if_c<
        eastl::is_move_constructible<T>::value &&
        eastl::is_assignable<T &, U>::value, T>
    exchange(T &t, U &&u)
        noexcept(
            eastl::is_nothrow_move_constructible<T>::value &&
            eastl::is_nothrow_assignable<T &, U>::value)
    {
        T tmp((T &&) t);
        t = (U &&) u;
        CPP_DIAGNOSTIC_IGNORE_INIT_LIST_LIFETIME
        return tmp;
    }

    /// \cond
    namespace adl_swap_detail
    {
        // Intentionally create an ambiguity with eastl::swap, which is (possibly) unconstrained.
        template<typename T>
        eastl::unused swap(T &, T &) = delete;

        template<typename T, eastl::size_t N>
        eastl::unused swap(T (&)[N], T (&)[N]) = delete;

        eastl::unused swap(eastl::argument_sink, eastl::argument_sink) = delete;

#ifdef CPP_WORKAROUND_MSVC_895622
        eastl::unused swap();
#endif

        template<typename T, typename U>
        decltype(swap(eastl::declval<T>(), eastl::declval<U>())) try_adl_swap_(int);

        template<typename T, typename U>
        eastl::unused try_adl_swap_(long);

        template<typename T, typename U = T>
        CPP_INLINE_VAR constexpr bool is_adl_swappable_v =
            !META_IS_SAME(decltype(adl_swap_detail::try_adl_swap_<T, U>(42)), eastl::unused);

        struct swap_fn
        {
            // Dispatch to user-defined swap found via ADL:
            template<typename T, typename U>
            CPP_CXX14_CONSTEXPR
            meta::if_c<is_adl_swappable_v<T, U>>
            operator()(T &&t, U &&u) const
            noexcept(noexcept(swap((T &&) t, (U &&) u)))
            {        
                swap((T &&) t, (U &&) u);
            }

            // For intrinsically swappable (i.e., movable) types for which
            // a swap overload cannot be found via ADL, swap by moving.
            template<typename T>
            CPP_CXX14_CONSTEXPR
            meta::if_c<
                !is_adl_swappable_v<T &> &&
                detail::is_movable_v<T>>
            operator()(T &a, T &b) const
            noexcept(noexcept(b = concepts::exchange(a, (T &&) b)))
            {
                b = concepts::exchange(a, (T &&) b);
            }

            // For arrays of intrinsically swappable (i.e., movable) types
            // for which a swap overload cannot be found via ADL, swap array
            // elements by moving.
            template<typename T, typename U, eastl::size_t N>
            CPP_CXX14_CONSTEXPR
            meta::if_c<
                !is_adl_swappable_v<T (&)[N], U (&)[N]> &&
                is_swappable_with<T &, U &>::value>
            operator()(T (&t)[N], U (&u)[N]) const
                noexcept(is_nothrow_swappable_with<T &, U &>::value)
            {
                for(eastl::size_t i = 0; i < N; ++i)
                    (*this)(t[i], u[i]);
            }

            // For rvalue pairs and tuples of swappable types, swap the
            // members. This permits code like:
            //   ranges::swap(eastl::tie(a,b,c), eastl::tie(d,e,f));
            template<typename F0, typename S0, typename F1, typename S1>
            CPP_CXX14_CONSTEXPR
            meta::if_c<is_swappable_with<F0, F1>::value && is_swappable_with<S0, S1>::value>
            operator()(eastl::pair<F0, S0> &&left, eastl::pair<F1, S1> &&right) const
                noexcept(
                    is_nothrow_swappable_with<F0, F1>::value &&
                    is_nothrow_swappable_with<S0, S1>::value)
            {
                swap_fn()(static_cast<eastl::pair<F0, S0> &&>(left).first,
                          static_cast<eastl::pair<F1, S1> &&>(right).first);
                swap_fn()(static_cast<eastl::pair<F0, S0> &&>(left).second,
                          static_cast<eastl::pair<F1, S1> &&>(right).second);
            }

            template<typename ...Ts, typename ...Us>
            CPP_CXX14_CONSTEXPR
            meta::if_c<meta::and_c<is_swappable_with<Ts, Us>::value...>::value>
            operator()(eastl::tuple<Ts...> && left, eastl::tuple<Us...> && right) const
                noexcept(meta::and_c<is_nothrow_swappable_with<Ts, Us>::value...>::value)
            {
                swap_fn::impl(static_cast<eastl::tuple<Ts...> &&>(left),
                              static_cast<eastl::tuple<Us...> &&>(right),
                    meta::make_index_sequence<sizeof...(Ts)>{});
            }

        private:
            template<typename... Ts>
            static constexpr int ignore_unused(Ts &&...)
            {
                return 0;
            }
            template<typename T, typename U, eastl::size_t ...Is>
            CPP_CXX14_CONSTEXPR
            static void impl(T &&left, U &&right, meta::index_sequence<Is...>)
            {
                (void) swap_fn::ignore_unused(
                    (swap_fn()(eastl::get<Is>(static_cast<T &&>(left)),
                               eastl::get<Is>(static_cast<U &&>(right))),
                     42)...);
            }
        };

        template<typename T, typename U, typename = void>
        struct is_swappable_with_
            : eastl::false_type
        {};

        template<typename T, typename U>
        struct is_swappable_with_<T, U, meta::void_<
            decltype(swap_fn()(eastl::declval<T>(), eastl::declval<U>())),
            decltype(swap_fn()(eastl::declval<U>(), eastl::declval<T>()))>>
            : eastl::true_type
        {};

        template<typename T, typename U>
        struct is_nothrow_swappable_with_
            : meta::bool_<noexcept(swap_fn()(eastl::declval<T>(), eastl::declval<U>())) &&
                        noexcept(swap_fn()(eastl::declval<U>(), eastl::declval<T>()))>
        {};

        // Q: Should eastl::reference_wrapper be considered a proxy wrt swapping rvalues?
        // A: No. Its operator= is currently defined to reseat the references, so
        //    eastl::swap(ra, rb) already means something when ra and rb are (lvalue)
        //    reference_wrappers. That reseats the reference wrappers but leaves the
        //    referents unmodified. Treating rvalue reference_wrappers differently would
        //    be confusing.

        // Q: Then why is it OK to "re"-define swap for pairs and tuples of references?
        // A: Because as defined above, swapping an rvalue tuple of references has the same
        //    semantics as swapping an lvalue tuple of references. Rather than reseat the
        //    references, assignment happens *through* the references.

        // Q: But I have an iterator whose operator* returns an rvalue
        //    eastl::reference_wrapper<T>. How do I make it model indirectly_swappable?
        // A: With an overload of iter_swap.
    }
    /// \endcond

    /// \ingroup group-utility
    template<typename T, typename U>
    struct is_swappable_with
        : adl_swap_detail::is_swappable_with_<T, U>
    {};

    /// \ingroup group-utility
    template<typename T, typename U>
    struct is_nothrow_swappable_with
        : meta::and_<
            is_swappable_with<T, U>,
            adl_swap_detail::is_nothrow_swappable_with_<T, U>>
    {};

    /// \ingroup group-utility
    template<typename T>
    struct is_swappable
        : is_swappable_with<T &, T &>
    {};

    /// \ingroup group-utility
    template<typename T>
    struct is_nothrow_swappable
        : is_nothrow_swappable_with<T &, T &>
    {};

    /// \ingroup group-utility
    /// \relates adl_swap_detail::swap_fn
    CPP_DEFINE_CPO(adl_swap_detail::swap_fn, swap)
}

#endif
