/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_CLAMP_HPP
#define EARANGES_ALGORITHM_CLAMP_HPP

#include <EASTL/initializer_list.h>
#include <EASTL/algorithm.h>// I don't like this giant include...

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(clamp)

            /// \brief function template \c clamp
            template(typename T, typename C = less, typename P = identity)(
                requires indirect_strict_weak_order<
                    C,
                    projected<T const *, P>>) constexpr T const &
            EARANGES_FUNC(clamp)(
                T const & val, T const & lo, T const & hi, C pred = C{}, P proj = P{}) //
            {
                return eastl::clamp(
                    invoke(proj, val), invoke(proj, lo), invoke(proj, hi), pred);
            }

            /// \overload
            template(typename Rng, typename C = less, typename P = identity)(
                requires input_range<Rng> AND
                    indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>
                        AND indirectly_copyable_storable<
                            iterator_t<Rng>,
                            range_value_t<Rng> *>) constexpr range_value_t<Rng> const & //
            EARANGES_FUNC(clamp)(Rng && rng,
                                 const range_value_t<Rng> & lo,
                                 const range_value_t<Rng> & hi,
                                 C pred = C{},
                                 P proj = P{}) //
            {
                auto first = ranges::begin(rng);
                const auto last = ranges::end(rng);
                EARANGES_EXPECT(first != last);
                while(++first != last)
                {
                    *first = clamp(*first, lo, hi, pred, proj);
                }
                return first;
            }

            /// \overload
            template(typename T, typename C = less, typename P = identity)(
                requires copyable<T> AND indirect_strict_weak_order<
                    C,
                    projected<T const *, P>>) constexpr range_value_t<Rng> const &
            EARANGES_FUNC(clamp)(std::initializer_list<T> const && rng,
                                 const range_value_t<Rng> & lo,
                                 const range_value_t<Rng> & hi,
                                 C pred = C{},
                                 P proj = P{}) //
            {
                return (*this)(rng, lo, hi, eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(clamp)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif