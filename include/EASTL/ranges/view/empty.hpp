/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_EMPTY_HPP
#define RANGES_V3_VIEW_EMPTY_HPP

#include "../range_fwd.hpp"

#include "../view/interface.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename T>
    struct empty_view : view_interface<empty_view<T>, (cardinality)0>
    {
        static_assert(eastl::is_object<T>::value, "The template parameter to empty_view must be an object type.");
        empty_view() = default;
        static constexpr T * begin() noexcept
        {
            return nullptr;
        }
        static constexpr T * end() noexcept
        {
            return nullptr;
        }
        static constexpr std::size_t size() noexcept
        {
            return 0u;
        }
        static constexpr T * data() noexcept
        {
            return nullptr;
        }
        RANGES_DEPRECATED(
            "Replace views::empty<T>() with views::empty<T>. "
            "It is now a variable template.")
        empty_view operator()() const
        {
            return *this;
        }
    };

    template<typename T>
    RANGES_INLINE_VAR constexpr bool enable_borrowed_range<empty_view<T>> = true;

    namespace views
    {
        template<typename T>
        RANGES_INLINE_VAR constexpr empty_view<T> empty{};
    }

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"
#include "../detail/satisfy_boost_range.hpp"
RANGES_SATISFY_BOOST_RANGE(::ranges::empty_view)

#endif