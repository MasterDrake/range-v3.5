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
#ifndef RANGES_V3_ALGORITHM_FILL_N_HPP
#define RANGES_V3_ALGORITHM_FILL_N_HPP
//TODO: Capire a cosa serve?
//#include <utility>

#include "../range_fwd.hpp"

#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(fill_n)

        /// \brief function template \c equal
        template(typename O, typename V)(requires output_iterator<O, V const &>)
        constexpr O RANGES_FUNC(fill_n)(O first, iter_difference_t<O> n, V const & val)
        {
            RANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(first);
            for(; n != 0; ++b, --n)
                *b = val;
            return recounted(first, b, norig);
        }

    RANGES_FUNC_END(fill_n)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif