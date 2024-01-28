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
#ifndef EARANGES_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define EARANGES_ALGORITHM_PARTIAL_SORT_COPY_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/heap_algorithm.hpp>
#include <EARanges/functional/comparisons.hpp>
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

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(partial_sort_copy)

            /// \brief function template \c partial_sort_copy
            template(typename I,
                     typename SI,
                     typename O,
                     typename SO,
                     typename C = less,
                     typename PI = identity,
                     typename PO =
                         identity)(requires input_iterator<I> AND sentinel_for<SI, I> AND
                                       random_access_iterator<O>
                                           AND sentinel_for<SO, O>
                                               AND indirectly_copyable<I, O>
                                                   AND sortable<O, C, PO>
                                                       AND indirect_strict_weak_order<
                                                           C,
                                                           projected<I, PI>,
                                                           projected<O, PO>>) constexpr O
            EARANGES_FUNC(partial_sort_copy)(I first,
                                             SI last,
                                             O out_begin,
                                             SO out_end,
                                             C pred = C{},
                                             PI in_proj = PI{},
                                             PO out_proj = PO{}) //
            {
                O r = out_begin;
                if(r != out_end)
                {
                    for(; first != last && r != out_end; ++first, ++r)
                        *r = *first;
                    make_heap(out_begin, r, ranges::ref(pred), ranges::ref(out_proj));
                    auto len = r - out_begin;
                    for(; first != last; ++first)
                    {
                        auto && x = *first;
                        if(invoke(pred, invoke(in_proj, x), invoke(out_proj, *out_begin)))
                        {
                            *out_begin = (decltype(x) &&)x;
                            detail::sift_down_n(out_begin,
                                                len,
                                                out_begin,
                                                ranges::ref(pred),
                                                ranges::ref(out_proj));
                        }
                    }
                    sort_heap(out_begin, r, ranges::ref(pred), ranges::ref(out_proj));
                }
                return r;
            }

            /// \overload
            template(typename InRng,
                     typename OutRng,
                     typename C = less,
                     typename PI = identity,
                     typename PO = identity)(
                requires input_range<InRng> AND random_access_range<OutRng> AND
                    indirectly_copyable<iterator_t<InRng>, iterator_t<OutRng>>
                        AND sortable<iterator_t<OutRng>, C, PO>
                            AND indirect_strict_weak_order<
                                C,
                                projected<iterator_t<InRng>, PI>,
                                projected<iterator_t<OutRng>,
                                          PO>>) constexpr borrowed_iterator_t<OutRng>
            EARANGES_FUNC(partial_sort_copy)(InRng && in_rng,
                                             OutRng && out_rng,
                                             C pred = C{},
                                             PI in_proj = PI{},
                                             PO out_proj = PO{}) //
            {
                return (*this)(begin(in_rng),
                               end(in_rng),
                               begin(out_rng),
                               end(out_rng),
                               eastl::move(pred),
                               eastl::move(in_proj),
                               eastl::move(out_proj));
            }

        EARANGES_FUNC_END(partial_sort_copy)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif