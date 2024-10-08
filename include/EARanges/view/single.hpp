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

#ifndef EARANGES_VIEW_SINGLE_HPP
#define EARANGES_VIEW_SINGLE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/addressof.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename T>
    struct single_view : view_interface<single_view<T>, (cardinality)1>
    {
    private:
        CPP_assert(copy_constructible<T>);
        static_assert(eastl::is_object<T>::value, "The template parameter of single_view must be an object type");
        semiregular_box_t<T> value_;
        template<typename... Args>
        constexpr single_view(in_place_t, eastl::true_type, Args &&... args)
          : value_{static_cast<Args &&>(args)...}
        {}
        template<typename... Args>
        constexpr single_view(in_place_t, eastl::false_type, Args &&... args)
          : value_{in_place, static_cast<Args &&>(args)...}
        {}

    public:
        single_view() = default;
        constexpr explicit single_view(T const & t)
          : value_(t)
        {}
        constexpr explicit single_view(T && t)
          : value_(eastl::move(t))
        {}
        template(class... Args)(
            requires constructible_from<T, Args...>)
            constexpr single_view(in_place_t, Args &&... args)
          : single_view{in_place,
                        meta::bool_<(bool)semiregular<T>>{},
                        static_cast<Args &&>(args)...}
        {}
        constexpr T * begin() noexcept
        {
            return data();
        }
        constexpr T const * begin() const noexcept
        {
            return data();
        }
        constexpr T * end() noexcept
        {
            return data() + 1;
        }
        constexpr T const * end() const noexcept
        {
            return data() + 1;
        }
        static constexpr eastl::size_t size() noexcept
        {
            return 1u;
        }
        constexpr T * data() noexcept
        {
            return detail::addressof(static_cast<T &>(value_));
        }
        constexpr T const * data() const noexcept
        {
            return detail::addressof(static_cast<T const &>(value_));
        }
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<class T>
    explicit single_view(T &&) //
        -> single_view<detail::decay_t<T>>;
#endif

    namespace views
    {
        struct single_fn
        {
            template(typename Val)(requires copy_constructible<Val>)
            single_view<Val> operator()(Val value) const
            {
                return single_view<Val>{eastl::move(value)};
            }
        };

        /// \relates single_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(single_fn, single)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::single_view)

#endif
