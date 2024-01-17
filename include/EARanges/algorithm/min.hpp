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
#ifndef EARANGES_ALGORITHM_MIN_HPP
#define EARANGES_ALGORITHM_MIN_HPP

#include <EASTL/initializer_list.h>

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

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(min)

        /// \brief function template \c min
        template(typename T, typename C = less, typename P = identity)(requires indirect_strict_weak_order<C, projected<T const *, P>>)
        constexpr T const & EARANGES_FUNC(min)(T const & a, T const & b, C pred = C{}, P proj = P{}) //
        {
            return invoke(pred, invoke(proj, b), invoke(proj, a)) ? b : a;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(
            requires input_range<Rng> AND
            indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>> AND
            indirectly_copyable_storable<iterator_t<Rng>, range_value_t<Rng> *>)
        constexpr range_value_t<Rng> //
        EARANGES_FUNC(min)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            auto first = ranges::begin(rng);
            auto last = ranges::end(rng);
            EARANGES_EXPECT(first != last);
            range_value_t<Rng> result = *first;
            while(++first != last)
            {
                auto && tmp = *first;
                if(invoke(pred, invoke(proj, tmp), invoke(proj, result)))
                    result = (decltype(tmp) &&)tmp;
            }
            return result;
        }

        /// \overload
        template(typename T, typename C = less, typename P = identity)(requires copyable<T> AND indirect_strict_weak_order<C, projected<T const *, P>>)
        constexpr T EARANGES_FUNC(min)(std::initializer_list<T> const && rng, C pred = C{}, P proj = P{}) //
        {
            return (*this)(rng, eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(min)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
