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
#ifndef EARANGES_ALGORITHM_COPY_IF_HPP
#define EARANGES_ALGORITHM_COPY_IF_HPP

#include "EASTL/type_traits.h"
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
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
    /// 
    EARANGES_FUNC_BEGIN(copy_if)

        /// \brief function template \c copy_if
        template(typename I, typename S, typename O, typename F, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                weakly_incrementable<O> AND
                indirect_unary_predicate<F, projected<I, P>> AND
                indirectly_copyable<I, O>)
        constexpr O //
        EARANGES_FUNC(copy_if)(I first, S last, O result, F pred, P proj = P{}) //
        {
            // This implementation's performance could be improved by taking a more
            // complicated approach like with the copy algorithm.

            for(; first != last; ++first)
            {
               if(invoke(pred, invoke(proj, *first)))
                   *result++ = *first;
            }
            return result;
        }

        /// \overload
        template(typename Rng, typename O, typename F, typename P = identity)(
            requires input_range<Rng> AND weakly_incrementable<O> AND
            indirect_unary_predicate<F, projected<iterator_t<Rng>, P>> AND
            indirectly_copyable<iterator_t<Rng>, O>)
        constexpr O //
        EARANGES_FUNC(copy_if)(Rng && rng, O out, F pred, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), eastl::move(out), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(copy_if)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif