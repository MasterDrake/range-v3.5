/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_FUNCTIONAL_INVOKE_HPP
#define EARANGES_FUNCTIONAL_INVOKE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/functional.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT
EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

#ifndef EARANGES_CONSTEXPR_INVOKE
#ifdef EARANGES_WORKAROUND_CLANG_23135
#define EARANGES_CONSTEXPR_INVOKE 0
#else
#define EARANGES_CONSTEXPR_INVOKE 1
#endif
#endif

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    /// \cond
    namespace detail
    {
        EARANGES_DIAGNOSTIC_PUSH
        EARANGES_DIAGNOSTIC_IGNORE_VOID_PTR_DEREFERENCE

        template<typename U>
        U & can_reference_(U &&);

        // clang-format off
        /// \concept dereferenceable_part_
        /// \brief The \c dereferenceable_part_ concept
        template<typename T>
        CPP_requires(dereferenceable_part_,
            requires(T && t) //
            (
                detail::can_reference_(*(T &&) t)
            ));
        /// \concept dereferenceable_
        /// \brief The \c dereferenceable_ concept
        template<typename T>
        CPP_concept dereferenceable_ = //
            CPP_requires_ref(detail::dereferenceable_part_, T);
        // clang-format on

        EARANGES_DIAGNOSTIC_POP

        template<typename T>
        EARANGES_INLINE_VAR constexpr bool is_reference_wrapper_v =
            meta::is<T, reference_wrapper>::value ||
            meta::is<T, eastl::reference_wrapper>::value;
    } // namespace detail
    /// \endcond

    template<typename T>
    EARANGES_INLINE_VAR constexpr bool is_reference_wrapper_v = detail::is_reference_wrapper_v<detail::decay_t<T>>;

    template<typename T>
    using is_reference_wrapper = meta::bool_<is_reference_wrapper_v<T>>;

    struct invoke_fn
    {
    private:
        template(typename, typename T1)(
            requires detail::dereferenceable_<T1>) static constexpr decltype(auto)
            coerce(T1 && t1, long) noexcept(noexcept(*static_cast<T1 &&>(t1)))
        {
            return *static_cast<T1 &&>(t1);
        }

        template(typename T, typename T1)(
            requires derived_from<detail::decay_t<T1>,
                                  T>) static constexpr T1 && coerce(T1 && t1,
                                                                    int) noexcept
        {
            return static_cast<T1 &&>(t1);
        }

        template(typename,
                 typename T1)(requires detail::is_reference_wrapper_v<
                              detail::decay_t<T1>>) static constexpr decltype(auto)
            coerce(T1 && t1, int) noexcept
        {
            return static_cast<T1 &&>(t1).get();
        }

    public:
        template<typename F, typename T, typename T1, typename... Args>
        constexpr auto operator()(F T::*f, T1 && t1, Args &&... args) const
            noexcept(noexcept((invoke_fn::coerce<T>((T1 &&)t1, 0).*f)((Args &&)args...)))
                -> decltype((invoke_fn::coerce<T>((T1 &&)t1, 0).*f)((Args &&)args...))
        {
            return (invoke_fn::coerce<T>((T1 &&)t1, 0).*f)((Args &&)args...);
        }

        template<typename D, typename T, typename T1>
        constexpr auto operator()(D T::*f, T1 && t1) const
            noexcept(noexcept(invoke_fn::coerce<T>((T1 &&)t1, 0).*f))
                -> decltype(invoke_fn::coerce<T>((T1 &&)t1, 0).*f)
        {
            return invoke_fn::coerce<T>((T1 &&)t1, 0).*f;
        }

        template<typename F, typename... Args>
        CPP_PP_IIF(EARANGES_CONSTEXPR_INVOKE)
        (CPP_PP_EXPAND, CPP_PP_EAT)(constexpr) auto operator()(F && f, Args &&... args) const
            noexcept(noexcept(((F &&)f)((Args &&)args...))) -> decltype(((F &&)f)((Args &&)args...))
        {
            return ((F &&)f)((Args &&)args...);
        }
    };

    EARANGES_INLINE_VARIABLE(invoke_fn, invoke)

#ifdef EARANGES_WORKAROUND_MSVC_701385
    /// \cond
    namespace detail
    {
        template<typename Void, typename Fun, typename... Args>
        struct _invoke_result_
        {};

        template<typename Fun, typename... Args>
        struct _invoke_result_<meta::void_<decltype(invoke(eastl::declval<Fun>(), eastl::declval<Args>()...))>, Fun, Args...>
        {
            using type = decltype(invoke(eastl::declval<Fun>(), eastl::declval<Args>()...));
        };
    } // namespace detail
    /// \endcond

    template<typename Fun, typename... Args>
    using invoke_result = detail::_invoke_result_<void, Fun, Args...>;

    template<typename Fun, typename... Args>
    using invoke_result_t = meta::_t<invoke_result<Fun, Args...>>;

#else  // EARANGES_WORKAROUND_MSVC_701385
    template<typename Fun, typename... Args>
    using invoke_result_t =
        decltype(invoke(eastl::declval<Fun>(), eastl::declval<Args>()...));

    template<typename Fun, typename... Args>
    struct invoke_result : meta::defer<invoke_result_t, Fun, Args...>
    {};
#endif // EARANGES_WORKAROUND_MSVC_701385

    /// \cond
    namespace detail
    {
        template<bool IsInvocable>
        struct is_nothrow_invocable_impl_
        {
            template<typename Fn, typename... Args>
            static constexpr bool apply() noexcept
            {
                return false;
            }
        };
        template<>
        struct is_nothrow_invocable_impl_<true>
        {
            template<typename Fn, typename... Args>
            static constexpr bool apply() noexcept
            {
                return noexcept(invoke(eastl::declval<Fn>(), eastl::declval<Args>()...));
            }
        };
    } // namespace detail
    /// \endcond


    template<typename Fn, typename... Args>
    EARANGES_INLINE_VAR constexpr bool is_invocable_v = meta::is_trait<invoke_result<Fn, Args...>>::value;

    template<typename Fn, typename... Args>
    EARANGES_INLINE_VAR constexpr bool is_nothrow_invocable_v =  detail::is_nothrow_invocable_impl_<is_invocable_v<Fn, Args...>>::template apply<Fn, Args...>();
    /// @}
} // namespace ranges

EARANGES_DIAGNOSTIC_POP

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_FUNCTIONAL_INVOKE_HPP
