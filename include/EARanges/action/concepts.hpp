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

#ifndef EARANGES_ACTION_CONCEPTS_HPP
#define EARANGES_ACTION_CONCEPTS_HPP

#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct movable_input_iterator
            {
                using iterator_category = eastl::input_iterator_tag;
                using value_type = T;
                using difference_type = std::ptrdiff_t;
                using pointer = T *;
                using reference = T &&;

                movable_input_iterator() = default;
                movable_input_iterator & operator++();
                movable_input_iterator operator++(int);
                bool operator==(movable_input_iterator const &) const;
                bool operator!=(movable_input_iterator const &) const;
                T && operator*() const;
            };
        } // namespace detail
          /// \endcond

        /// \addtogroup group-range
        /// @{

        // clang-format off
    /// \concept semi_container
    /// \brief The \c semi_container concept
    /// \c eastl::array is a \c semi_container, native arrays are not.
    template<typename T>
    CPP_concept semi_container =
        forward_range<T> && default_constructible<uncvref_t<T>> &&
        movable<uncvref_t<T>> &&
        !view_<T>;

    /// \concept container_
    /// \brief The \c container_ concept
    /// \c eastl::vector is a container, \c eastl::array is not
    template(typename T)(
    concept (container_)(T),
        constructible_from<
            uncvref_t<T>,
            detail::movable_input_iterator<range_value_t<T>>,
            detail::movable_input_iterator<range_value_t<T>>>
    );

    /// \concept container
    /// \brief The \c container concept
    template<typename T>
    CPP_concept container = semi_container<T> && CPP_concept_ref(ranges::container_, T);

    /// \concept reservable_
    /// \brief The \c reservable_ concept
    template<typename C>
    CPP_requires(reservable_,
        requires(C & c, C const & cc) //
        (
            c.reserve(ranges::size(c)),
            cc.capacity(),
            cc.max_size(),
            concepts::requires_<same_as<decltype(cc.capacity()), decltype(ranges::size(c))>>,
            concepts::requires_<same_as<decltype(cc.max_size()), decltype(ranges::size(c))>>
        ));

    /// \concept reservable
    /// \brief The \c reservable concept
    template<typename C>
    CPP_concept reservable = container<C> && sized_range<C> && CPP_requires_ref(ranges::reservable_, C);

    /// \concept reservable_with_assign_
    /// \brief The \c reservable_with_assign_ concept
    template<typename C, typename I>
    CPP_requires(reservable_with_assign_, requires(C & c, I i) //
        (
            c.assign(i, i)
        ));
    /// \concept reservable_with_assign
    /// \brief The \c reservable_with_assign concept
    template<typename C, typename I>
    CPP_concept reservable_with_assign =
        reservable<C> && //
        input_iterator<I> && //
        CPP_requires_ref(ranges::reservable_with_assign_, C, I);

    /// \concept random_access_reservable
    /// \brief The \c random_access_reservable concept
    template<typename C>
    CPP_concept random_access_reservable = reservable<C> && random_access_range<C>;
        // clang-format on

        /// \cond
        namespace detail
        {
            template(typename T)(requires container<T>) eastl::true_type
                is_lvalue_container_like(T &) noexcept
            {
                return {};
            }

            template(typename T)(
                requires container<T>) meta::not_<eastl::is_rvalue_reference<T>> //
                is_lvalue_container_like(reference_wrapper<T>) noexcept
            {
                return {};
            }

            template(typename T)(requires container<T>) eastl::true_type
                is_lvalue_container_like(eastl::reference_wrapper<T>) noexcept
            {
                return {};
            }

            template(typename T)(requires container<T>) eastl::true_type
                is_lvalue_container_like(ref_view<T>) noexcept
            {
                return {};
            }

            template<typename T>
            using is_lvalue_container_like_t =
                decltype(detail::is_lvalue_container_like(eastl::declval<T>()));

        } // namespace detail
          /// \endcond

        // clang-format off
    /// \concept lvalue_container_like_
    /// \brief The \c lvalue_container_like_ concept
    template(typename T)(concept (lvalue_container_like_)(T), implicitly_convertible_to<detail::is_lvalue_container_like_t<T>, eastl::true_type>);
    /// \concept lvalue_container_like
    /// \brief The \c lvalue_container_like concept
    template<typename T>
    CPP_concept lvalue_container_like = forward_range<T> && CPP_concept_ref(ranges::lvalue_container_like_, T);
        // clang-format on
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
