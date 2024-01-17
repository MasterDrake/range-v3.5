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

#ifndef EARANGES_RANGE_TRAITS_HPP
#define EARANGES_RANGE_TRAITS_HPP

#include <EASTL/iterator.h>
#include <EASTL/type_traits.h>
#include <EASTL/array.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/range/access.hpp>
#include <EARanges/range/primitives.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename I, typename S>
        using common_iterator_impl_t = enable_if_t<(bool)(input_or_output_iterator<I> && sentinel_for<S, I>), common_iterator<I, S>>;
    }
    /// \endcond

    /// \addtogroup group-range
    /// @{
    template<typename I, typename S>
    using common_iterator_t = meta::conditional_t<eastl::is_same<I, S>::value, I, detail::common_iterator_impl_t<I, S>>;

    /// \cond
    namespace detail
    {
        template<typename I, typename S>
        using cpp17_iterator_t = meta::conditional_t<eastl::is_integral<iter_difference_t<I>>::value, common_iterator_t<I, S>, cpp17_iterator<common_iterator_t<I, S>>>;
    }
    /// \endcond

    // Aliases (SFINAE-able)
    template<typename Rng>
    using range_difference_t = iter_difference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_value_t = iter_value_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_reference_t = iter_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_common_reference_t = iter_common_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_size_t = decltype(ranges::size(eastl::declval<Rng &>()));

    template<typename Rng>
    using range_common_iterator_t = common_iterator_t<iterator_t<Rng>, sentinel_t<Rng>>;

    /// \cond
    namespace detail
    {
        template<typename Rng>
        using range_cpp17_iterator_t = cpp17_iterator_t<iterator_t<Rng>, sentinel_t<Rng>>;

        eastl::integral_constant<cardinality, finite> test_cardinality(void *);
        template<cardinality Card>
        eastl::integral_constant<cardinality, Card> test_cardinality(basic_view<Card> *);
        template<typename T, std::size_t N>
        eastl::integral_constant<cardinality, static_cast<cardinality>(N)> test_cardinality(T (*)[N]);
        template<typename T, std::size_t N>
        eastl::integral_constant<cardinality, static_cast<cardinality>(N)> test_cardinality(eastl::array<T, N> *);
    } // namespace detail
    /// \endcond

    // User customization point for specifying the cardinality of ranges:
    template<typename Rng, typename Void /*= void*/>
    struct range_cardinality
      : meta::conditional_t<EARANGES_IS_SAME(Rng, uncvref_t<Rng>),
                          decltype(detail::test_cardinality(
                              static_cast<uncvref_t<Rng> *>(nullptr))),
                          range_cardinality<uncvref_t<Rng>>>
    {};

} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
