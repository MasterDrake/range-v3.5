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
#ifndef EARANGES_ALGORITHM_PARTITION_COPY_HPP
#define EARANGES_ALGORITHM_PARTITION_COPY_HPP

#include <EASTL/tuple.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
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
        /// \addtogroup group-algorithms
        /// @{
        template<typename I, typename O0, typename O1>
        using partition_copy_result = detail::in_out1_out2_result<I, O0, O1>;

        EARANGES_FUNC_BEGIN(partition_copy)

            /// \brief function template \c partition_copy
            template(typename I,
                     typename S,
                     typename O0,
                     typename O1,
                     typename C,
                     typename P = identity)(
                requires input_iterator<I> AND sentinel_for<S, I> AND
                    weakly_incrementable<O0>
                        AND weakly_incrementable<O1>
                            AND indirectly_copyable<I, O0>
                                AND indirectly_copyable<I, O1>
                                    AND indirect_unary_predicate<
                                        C,
                                        projected<I,
                                                  P>>) constexpr partition_copy_result<I,
                                                                                       O0,
                                                                                       O1>
            EARANGES_FUNC(partition_copy)(
                I first, S last, O0 o0, O1 o1, C pred, P proj = P{})
            {
                for(; first != last; ++first)
                {
                    auto && x = *first;
                    if(invoke(pred, invoke(proj, x)))
                    {
                        *o0 = (decltype(x) &&)x;
                        ++o0;
                    }
                    else
                    {
                        *o1 = (decltype(x) &&)x;
                        ++o1;
                    }
                }
                return {first, o0, o1};
            }

            /// \overload
            template(typename Rng,
                     typename O0,
                     typename O1,
                     typename C,
                     typename P = identity)(
                requires input_range<Rng> AND weakly_incrementable<O0> AND weakly_incrementable<
                    O1>
                    AND indirectly_copyable<iterator_t<Rng>, O0>
                        AND indirectly_copyable<iterator_t<Rng>, O1>
                            AND indirect_unary_predicate<
                                C,
                                projected<
                                    iterator_t<Rng>,
                                    P>>) constexpr partition_copy_result<borrowed_iterator_t<Rng>,
                                                                         O0,
                                                                         O1> //
            EARANGES_FUNC(partition_copy)(Rng && rng, O0 o0, O1 o1, C pred, P proj = P{})
            {
                return (*this)(begin(rng),
                               end(rng),
                               eastl::move(o0),
                               eastl::move(o1),
                               eastl::move(pred),
                               eastl::move(proj));
            }

        EARANGES_FUNC_END(partition_copy)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif