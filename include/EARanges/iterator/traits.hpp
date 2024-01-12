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

#ifndef EARANGES_ITERATOR_TRAITS_HPP
#define EARANGES_ITERATOR_TRAITS_HPP

#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/access.hpp> // for iter_move, iter_swap
#include <EARanges/utility/common_type.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    struct contiguous_iterator_tag : eastl::random_access_iterator_tag
    {};

    /// \cond
    namespace detail
    {
        template<typename I, typename = iter_reference_t<I>, typename R = decltype(iter_move(eastl::declval<I &>())), typename = R &>
        using iter_rvalue_reference_t = R;

        template<typename I>
        EARANGES_INLINE_VAR constexpr bool has_nothrow_iter_move_v = noexcept(iter_rvalue_reference_t<I>(ranges::iter_move(eastl::declval<I &>())));
    } // namespace detail
    /// \endcond

    template<typename I>
    using iter_rvalue_reference_t = detail::iter_rvalue_reference_t<I>;

    template<typename I>
    using iter_common_reference_t = common_reference_t<iter_reference_t<I>, iter_value_t<I> &>;

#if defined(EARANGES_DEEP_STL_INTEGRATION) && EARANGES_DEEP_STL_INTEGRATION && \
    !defined(EARANGES_DOXYGEN_INVOKED)
    template<typename T>
    using iter_difference_t =
        typename meta::conditional_t<detail::is_std_iterator_traits_specialized_v<T>,
                                   eastl::iterator_traits<uncvref_t<T>>,
                                   incrementable_traits<uncvref_t<T>>>::difference_type;
#else
    template<typename T>
    using iter_difference_t = typename incrementable_traits<uncvref_t<T>>::difference_type;
#endif

    // Defined in <range/v3/iterator/access.hpp>
    // template<typename T>
    // using iter_value_t = ...

    // Defined in <range/v3/iterator/access.hpp>
    // template<typename R>
    // using iter_reference_t = detail::iter_reference_t_<R>;

    // Defined in <range/v3/range_fwd.hpp>:
    // template<typename S, typename I>
    // inline constexpr bool disable_sized_sentinel = false;

    /// \cond
    namespace detail
    {
        template<typename I>
        using iter_size_t =
            meta::_t<meta::conditional_t<eastl::is_integral<iter_difference_t<I>>::value,
                               eastl::make_unsigned<iter_difference_t<I>>,
                               meta::id<iter_difference_t<I>>>>;

        template<typename I>
        using iter_arrow_t = decltype(eastl::declval<I &>().operator->());

        template<typename I>
        using iter_pointer_t =
            meta::_t<meta::conditional_t<
                        meta::is_trait<meta::defer<iter_arrow_t, I>>::value,
                        meta::defer<iter_arrow_t, I>,
                        eastl::add_pointer<iter_reference_t<I>>>>;

        template<typename T>
        struct difference_type_ : meta::defer<iter_difference_t, T>
        {};

        template<typename T>
        struct value_type_ : meta::defer<iter_value_t, T>
        {};

        template<typename T>
        struct size_type_ : meta::defer<iter_size_t, T>
        {};
    } // namespace detail

    /// \endcond

} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ITERATOR_TRAITS_HPP