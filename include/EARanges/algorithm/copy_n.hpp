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
#ifndef EARANGES_ALGORITHM_COPY_N_HPP
#define EARANGES_ALGORITHM_COPY_N_HPP

#include <EASTL/functional.h>
#include <EASTL/tuple.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using copy_n_result = detail::in_out_result<I, O>;

    EARANGES_FUNC_BEGIN(copy_n)

        /// \brief function template \c copy_n
        template(typename I, typename O, typename P = identity)(requires input_iterator<I> AND weakly_incrementable<O> AND indirectly_copyable<I, O>)
        constexpr copy_n_result<I, O> EARANGES_FUNC(copy_n)(I first, iter_difference_t<I> n, O out)
        {
            EARANGES_EXPECT(0 <= n);
            auto norig = n;
            auto b = uncounted(first);
            for(; n != 0; ++b, ++out, --n)
                *out = *b;
            return {recounted(first, b, norig), out};
        }

    EARANGES_FUNC_END(copy_n)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif