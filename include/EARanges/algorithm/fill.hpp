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
#ifndef EARANGES_ALGORITHM_FILL_HPP
#define EARANGES_ALGORITHM_FILL_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(fill)

        /// \brief function template \c fill
        template(typename O, typename S, typename V)(requires output_iterator<O, V const &> AND sentinel_for<S, O>)
        constexpr O EARANGES_FUNC(fill)(O first, S last, V const & val) //
        {
            for(; first != last; ++first)
                *first = val;
            return first;
        }

        /// \overload
        template(typename Rng, typename V)(requires output_range<Rng, V const &>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(fill)(Rng && rng, V const & val)
        {
            return (*this)(begin(rng), end(rng), val);
        }

    EARANGES_FUNC_END(fill)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif