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
#ifndef EARANGES_ALGORITHM_FILL_N_HPP
#define EARANGES_ALGORITHM_FILL_N_HPP

//TODO: What's for?
//#include <utility>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(fill_n)

        /// \brief function template \c equal
        template(typename O, typename V)(requires output_iterator<O, V const &>)
        constexpr O EARANGES_FUNC(fill_n)(O first, iter_difference_t<O> n, V const & val)
        {
            EARANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(first);
            for(; n != 0; ++b, --n)
                *b = val;
            return recounted(first, b, norig);
        }

    EARANGES_FUNC_END(fill_n)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif