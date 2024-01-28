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
#ifndef EARANGES_NUMERIC_IOTA_HPP
#define EARANGES_NUMERIC_IOTA_HPP

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-numerics
        /// @{
        struct iota_fn
        {
            template(typename O, typename S, typename T)(
                requires output_iterator<O, T const &> AND sentinel_for<S, O> AND
                    weakly_incrementable<T>) O
            operator()(O first, S last, T val) const
            {
                for(; first != last; ++first, ++val)
                    *first = detail::as_const(val);
                return first;
            }

            template(typename Rng,
                     typename T)(requires output_range<Rng, T const &> AND
                                     weakly_incrementable<T>) borrowed_iterator_t<Rng>
            operator()(Rng && rng, T val) const //
            {
                return (*this)(begin(rng), end(rng), detail::move(val));
            }
        };

        EARANGES_INLINE_VARIABLE(iota_fn, iota)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif