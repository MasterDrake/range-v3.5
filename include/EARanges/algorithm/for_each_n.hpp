/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Rostislav Khlebnikov 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_FOR_EACH_N_HPP
#define EARANGES_ALGORITHM_FOR_EACH_N_HPP

#include <EASTL/functional.h>
#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(for_each_n)

        /// \brief function template \c for_each_n
        template(typename I, typename F, typename P = identity)(requires input_iterator<I> AND indirectly_unary_invocable<F, projected<I, P>>)
        constexpr I EARANGES_FUNC(for_each_n)(I first, iter_difference_t<I> n, F fun, P proj = P{})
        {
            return eastl::for_each_n(first, n, eastl::move(fun), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirectly_unary_invocable<F, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(for_each_n)(Rng && rng, range_difference_t<Rng> n, F fun, P proj = P{})
        {
            if(sized_range<Rng>)
                EARANGES_EXPECT(n <= distance(rng));

            return (*this)(begin(rng), n, detail::move(fun), detail::move(proj));
        }

    EARANGES_FUNC_END(for_each_n)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif