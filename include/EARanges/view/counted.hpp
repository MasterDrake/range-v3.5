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
#ifndef EARANGES_VIEW_COUNTED_HPP
#define EARANGES_VIEW_COUNTED_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/counted_iterator.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/interface.hpp>
#include <EARanges/view/subrange.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename I>
    struct counted_view : view_interface<counted_view<I>, finite>
    {
    private:
        friend range_access;
        I it_;
        iter_difference_t<I> n_;

    public:
        counted_view() = default;
        counted_view(I it, iter_difference_t<I> n)
          : it_(it)
          , n_(n)
        {
            EARANGES_EXPECT(0 <= n_);
        }
        counted_iterator<I> begin() const
        {
            return make_counted_iterator(it_, n_);
        }
        default_sentinel_t end() const
        {
            return {};
        }
        auto size() const
        {
            return static_cast<detail::iter_size_t<I>>(n_);
        }
    };

    template<typename I>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<counted_view<I>> = true;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename I>
    counted_view(I, iter_difference_t<I>) -> counted_view<I>;
#endif

    namespace views
    {	//TODO: it's not used...
        /*struct cpp20_counted_fn
        {
            template(typename I)(requires input_or_output_iterator<I> AND (!random_access_iterator<I>)) //
            subrange<counted_iterator<I>, default_sentinel_t> //
            operator()(I it, iter_difference_t<I> n) const
            {
                return {make_counted_iterator(eastl::move(it), n), default_sentinel};
            }
            template(typename I)(requires random_access_iterator<I>)
            subrange<I> operator()(I it, iter_difference_t<I> n) const
            {
                return {it, it + n};
            }
        };*/

        struct counted_fn
        {
            template(typename I)(requires input_or_output_iterator<I> AND (!random_access_iterator<I>)) //
            counted_view<I> operator()(I it, iter_difference_t<I> n) const
            {
                return {eastl::move(it), n};
            }
            template(typename I)(requires random_access_iterator<I>)
            subrange<I> operator()(I it, iter_difference_t<I> n) const
            {
                return {it, it + n};
            }
        };

        /// \relates counted_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(counted_fn, counted)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::counted_view)

#endif