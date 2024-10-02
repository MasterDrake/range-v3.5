/// \file
// Range v3 library
//
//  Copyright Casey Carter 2017
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_LINEAR_DISTRIBUTE_HPP
#define EARANGES_VIEW_LINEAR_DISTRIBUTE_HPP

#include <EASTL/type_traits.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace views
    {
        /// \addtogroup group-views
        /// @{

        template<typename T>
        struct linear_distribute_view : view_facade<linear_distribute_view<T>, finite>
        {
            CPP_assert(eastl::is_arithmetic<T>());

        private:
            friend range_access;
            using Calc = meta::conditional_t<eastl::is_floating_point<T>::value, T, double>;

            T from_, to_;
            Calc delta_;
            std::ptrdiff_t n_;

            constexpr T read() const noexcept
            {
                return from_;
            }
            constexpr bool equal(default_sentinel_t) const noexcept
            {
                return n_ == 0;
            }
            constexpr bool equal(linear_distribute_view const & other) const noexcept
            {
                bool const eq = n_ == other.n_;
                EARANGES_DIAGNOSTIC_PUSH
                EARANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
                EARANGES_EXPECT(to_ == other.to_);
                EARANGES_EXPECT(!eq || from_ == other.from_);
                EARANGES_DIAGNOSTIC_POP
                return eq;
            }
            constexpr void next() noexcept
            {
                EARANGES_EXPECT(n_ > 0);
                --n_;
                if(n_ == 0)
                {
                    from_ = to_;
                }
                else
                {
                    from_ = T(to_ - (delta_ * Calc(n_ - 1)));
                }
            }

        public:
            constexpr linear_distribute_view() = default;
            constexpr linear_distribute_view(T from, T to, std::ptrdiff_t n) noexcept
              : from_(from)
              , to_(to)
              , delta_(n > 1 ? (to - from) / Calc(n - 1) : 0)
              , n_(n)
            {
                EARANGES_EXPECT(n_ > 0);
                EARANGES_EXPECT(to_ >= from_);
            }
            constexpr eastl::size_t size() const noexcept
            {
                return static_cast<eastl::size_t>(n_);
            }
        };

        /// Distributes `n` values linearly in the closed interval [`from`, `to`].
        ///
        /// \pre `from <= to && n > 0`
        ///
        /// If `from == to`, returns n-times `to`.
        /// If `n == 1` returns `to`.
        struct linear_distribute_fn
        {
            template(typename T)(requires eastl::is_arithmetic<T>::value)
            constexpr auto operator()(T from, T to, std::ptrdiff_t n) const
            {
                return linear_distribute_view<T>{from, to, n};
            }
        };

        /// \relates linear_distribute_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(linear_distribute_fn, linear_distribute)
    } // namespace views
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif