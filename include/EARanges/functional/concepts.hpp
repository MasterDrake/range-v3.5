
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
#ifndef EARANGES_FUNCTIONAL_CONCEPTS_HPP
#define EARANGES_FUNCTIONAL_CONCEPTS_HPP

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/functional/invoke.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{

    // clang-format off
    // WORKAROUND mysterious msvc bug
#if defined(_MSC_VER) && !defined(__clang__)
    /// \concept invocable
    /// \brief The \c invocable concept
    template<typename Fun, typename... Args>
    CPP_concept invocable = eastl::is_invocable_v<Fun, Args...>;
#else
    /// \concept invocable_
    /// \brief The \c invocable_ concept
    template<typename Fun, typename... Args>
    CPP_requires(invocable_,
        requires(Fun && fn) //
        (
            invoke((Fun &&) fn, eastl::declval<Args>()...)
        ));
    /// \concept invocable
    /// \brief The \c invocable concept
    template<typename Fun, typename... Args>
    CPP_concept invocable =
        CPP_requires_ref(ranges::invocable_, Fun, Args...);
#endif

    /// \concept regular_invocable
    /// \brief The \c regular_invocable concept
    template<typename Fun, typename... Args>
    CPP_concept regular_invocable =
        invocable<Fun, Args...>;
        // Axiom: equality_preserving(invoke(f, args...))

    /// \concept predicate_
    /// \brief The \c predicate_ concept
    template<typename Fun, typename... Args>
    CPP_requires(predicate_,
        requires(Fun && fn) //
        (
            concepts::requires_< convertible_to<decltype(invoke((Fun &&) fn, eastl::declval<Args>()...)), bool>>
        ));
    /// \concept predicate
    /// \brief The \c predicate concept
    template<typename Fun, typename... Args>
    CPP_concept predicate =
        regular_invocable<Fun, Args...> &&
        CPP_requires_ref(ranges::predicate_, Fun, Args...);

    /// \concept relation
    /// \brief The \c relation concept
    template<typename R, typename T, typename U>
    CPP_concept relation =
        predicate<R, T, T> &&
        predicate<R, U, U> &&
        predicate<R, T, U> &&
        predicate<R, U, T>;

    /// \concept strict_weak_order
    /// \brief The \c strict_weak_order concept
    template<typename R, typename T, typename U>
    CPP_concept strict_weak_order = relation<R, T, U>;
    // clang-format on

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif