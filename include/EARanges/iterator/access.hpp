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

#ifndef EARANGES_ITERATOR_ACCESS_HPP
#define EARANGES_ITERATOR_ACCESS_HPP

#include <EASTL/iterator.h>
#include <EASTL/type_traits.h>
//#include <utility>

#include <EARanges/std/detail/associated_types.hpp>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/move.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/utility/swap.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I,
#ifdef EARANGES_WORKAROUND_MSVC_683388
                 typename R = meta::conditional_t<
                     eastl::is_pointer<uncvref_t<I>>::value &&
                         eastl::is_array<eastl::remove_pointer_t<uncvref_t<I>>>::value,
                     eastl::add_lvalue_reference_t<eastl::remove_pointer_t<uncvref_t<I>>>,
                     decltype(*eastl::declval<I &>())>,
#else
                 typename R = decltype(*eastl::declval<I &>()),
#endif
                 typename = R &>
        using iter_reference_t_ = R;

        template<typename T>
        using iter_value_t_ = typename indirectly_readable_traits<T>::value_type;
    } // namespace detail
    /// \endcond

    template<typename R>
    using iter_reference_t = detail::iter_reference_t_<R>;

    template<typename R>
    using iter_value_t = detail::iter_value_t_<uncvref_t<R>>;

    /// \cond
    namespace _iter_move_
    {
#if EARANGES_BROKEN_CPO_LOOKUP
        void iter_move(); // unqualified name lookup block
#endif

        template<typename T>
        decltype(iter_move(eastl::declval<T>())) try_adl_iter_move_(int);

        template<typename T>
        void try_adl_iter_move_(long);

        template<typename T>
        EARANGES_INLINE_VAR constexpr bool is_adl_indirectly_movable_v = !EARANGES_IS_SAME(void, decltype(_iter_move_::try_adl_iter_move_<T>(42)));

        struct fn
        {
            // clang-format off
            template<typename I, typename = detail::enable_if_t<is_adl_indirectly_movable_v<I &>>>
#ifndef EARANGES_WORKAROUND_CLANG_23135
            constexpr
#endif // EARANGES_WORKAROUND_CLANG_23135
            auto CPP_auto_fun(operator())(I &&i)(const)
            (
                return iter_move(i)
            )

            template<
                typename I,
                typename = detail::enable_if_t<!is_adl_indirectly_movable_v<I &>>,
                typename R = iter_reference_t<I>>
#ifndef EARANGES_WORKAROUND_CLANG_23135
            constexpr
#endif // EARANGES_WORKAROUND_CLANG_23135
            auto CPP_auto_fun(operator())(I &&i)(const)
            (
                return static_cast<aux::move_t<R>>(aux::move(*i))
            )
            // clang-format on
        };
    } // namespace _iter_move_
    /// \endcond

    EARANGES_DEFINE_CPO(_iter_move_::fn, iter_move)

    /// \cond
    namespace detail
    {
        template<typename I, typename O>
        auto is_indirectly_movable_(I & (*i)(), O & (*o)(), iter_value_t<I> * v = nullptr)
            -> always_<eastl::true_type,
                       decltype(iter_value_t<I>(iter_move(i()))),
                       decltype(*v = iter_move(i())),
                       decltype(*o() = (iter_value_t<I> &&) * v),
                       decltype(*o() = iter_move(i()))>;
        template<typename I, typename O>
        auto is_indirectly_movable_(...) -> eastl::false_type;

        template<typename I, typename O>
        auto is_nothrow_indirectly_movable_(iter_value_t<I> * v) -> meta::bool_<
            noexcept(iter_value_t<I>(iter_move(eastl::declval<I &>()))) &&
            noexcept(*v = iter_move(eastl::declval<I &>())) &&
            noexcept(*eastl::declval<O &>() = (iter_value_t<I> &&)*v) &&
            noexcept(*eastl::declval<O &>() = iter_move(eastl::declval<I &>()))>;
        template<typename I, typename O>
        auto is_nothrow_indirectly_movable_(...) -> eastl::false_type;
    } // namespace detail
    /// \endcond

    template<typename I, typename O>
    EARANGES_INLINE_VAR constexpr bool is_indirectly_movable_v =
        decltype(detail::is_indirectly_movable_<I, O>(nullptr, nullptr))::value;

    template<typename I, typename O>
    EARANGES_INLINE_VAR constexpr bool is_nothrow_indirectly_movable_v =
        decltype(detail::is_nothrow_indirectly_movable_<I, O>(nullptr))::value;

    template<typename I, typename O>
    struct is_indirectly_movable : meta::bool_<is_indirectly_movable_v<I, O>>
    {};

    template<typename I, typename O>
    struct is_nothrow_indirectly_movable
      : meta::bool_<is_nothrow_indirectly_movable_v<I, O>>
    {};

    /// \cond
    namespace _iter_swap_
    {
        struct nope
        {};

        // Q: Should eastl::reference_wrapper be considered a proxy wrt swapping rvalues?
        // A: No. Its operator= is currently defined to reseat the references, so
        //    eastl::swap(ra, rb) already means something when ra and rb are (lvalue)
        //    reference_wrappers. That reseats the reference wrappers but leaves the
        //    referents unmodified. Treating rvalue reference_wrappers differently would
        //    be confusing.

        // Q: Then why is it OK to "re"-define swap for pairs and tuples of references?
        // A: Because as defined above, swapping an rvalue tuple of references has the
        //    same semantics as swapping an lvalue tuple of references. Rather than
        //    reseat the references, assignment happens *through* the references.

        // Q: But I have an iterator whose operator* returns an rvalue
        //    eastl::reference_wrapper<T>. How do I make it model indirectly_swappable?
        // A: With an overload of iter_swap.

        // Intentionally create an ambiguity with eastl::iter_swap, which is
        // unconstrained.
        template<typename T, typename U>
        nope iter_swap(T, U) = delete;

#ifdef EARANGES_WORKAROUND_MSVC_895622
        nope iter_swap();
#endif

        template<typename T, typename U>
        decltype(iter_swap(eastl::declval<T>(), eastl::declval<U>())) try_adl_iter_swap_(int);

        template<typename T, typename U>
        nope try_adl_iter_swap_(long);

        // Test whether an overload of iter_swap for a T and a U can be found
        // via ADL with the iter_swap overload above participating in the
        // overload set. This depends on user-defined iter_swap overloads
        // being a better match than the overload in namespace eastl.
        template<typename T, typename U>
        EARANGES_INLINE_VAR constexpr bool is_adl_indirectly_swappable_v =
            !EARANGES_IS_SAME(nope, decltype(_iter_swap_::try_adl_iter_swap_<T, U>(42)));

        struct fn
        {
            // *If* a user-defined iter_swap is found via ADL, call that:
            template<typename T, typename U>
            constexpr detail::enable_if_t<is_adl_indirectly_swappable_v<T, U>> operator()(T && t, U && u) const noexcept(noexcept(iter_swap((T &&) t, (U &&) u)))
            {
                (void)iter_swap((T &&) t, (U &&) u);
            }

            // *Otherwise*, for readable types with swappable reference
            // types, call ranges::swap(*a, *b)
            template<typename I0, typename I1>
            constexpr detail::enable_if_t<!is_adl_indirectly_swappable_v<I0, I1> && is_swappable_with<iter_reference_t<I0>, iter_reference_t<I1>>::value>
            operator()(I0 && a, I1 && b) const noexcept(noexcept(ranges::swap(*a, *b)))
            {
                ranges::swap(*a, *b);
            }

            // *Otherwise*, for readable types that are mutually
            // indirectly_movable_storable, implement as:
            //      iter_value_t<T0> tmp = iter_move(a);
            //      *a = iter_move(b);
            //      *b = eastl::move(tmp);
            template<typename I0, typename I1>
            constexpr detail::enable_if_t<
                !is_adl_indirectly_swappable_v<I0, I1> &&
                !is_swappable_with<iter_reference_t<I0>, iter_reference_t<I1>>::value &&
                is_indirectly_movable_v<I0, I1> && is_indirectly_movable_v<I1, I0>>
            operator()(I0 && a, I1 && b) const
                noexcept(is_nothrow_indirectly_movable_v<I0, I1> &&
                             is_nothrow_indirectly_movable_v<I1, I0>)
            {
                iter_value_t<I0> v0 = iter_move(a);
                *a = iter_move(b);
                *b = detail::move(v0);
            }
        };
    } // namespace _iter_swap_
    /// \endcond

    /// \relates _iter_swap_::fn
    EARANGES_DEFINE_CPO(_iter_swap_::fn, iter_swap)

    /// \cond
    namespace detail
    {
        template<typename T, typename U>
        auto is_indirectly_swappable_(T & (*t)(), U & (*u)())
            -> detail::always_<eastl::true_type, decltype(iter_swap(t(), u()))>;
        template<typename T, typename U>
        auto is_indirectly_swappable_(...) -> eastl::false_type;

        template<typename T, typename U>
        auto is_nothrow_indirectly_swappable_(int)
            -> meta::bool_<noexcept(iter_swap(eastl::declval<T &>(), eastl::declval<U &>()))>;
        template<typename T, typename U>
        auto is_nothrow_indirectly_swappable_(long) -> eastl::false_type;
    } // namespace detail
    /// \endcond

    template<typename T, typename U>
    EARANGES_INLINE_VAR constexpr bool is_indirectly_swappable_v =
        decltype(detail::is_indirectly_swappable_<T, U>(nullptr, nullptr))::value;

    template<typename T, typename U>
    EARANGES_INLINE_VAR constexpr bool is_nothrow_indirectly_swappable_v =
        decltype(detail::is_nothrow_indirectly_swappable_<T, U>(0))::value;

    template<typename T, typename U>
    struct is_indirectly_swappable : meta::bool_<is_indirectly_swappable_v<T, U>>
    {};

    template<typename T, typename U>
    struct is_nothrow_indirectly_swappable
      : meta::bool_<is_nothrow_indirectly_swappable_v<T, U>>
    {};

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ITERATOR_ACCESS_HPP
