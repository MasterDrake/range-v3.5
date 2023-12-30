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
#ifndef EARANGES_ALGORITHM_MINMAX_HPP
#define EARANGES_ALGORITHM_MINMAX_HPP

#include <EASTL/initializer_list.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
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
    template<typename T>
    using minmax_result = detail::min_max_result<T, T>;

    EARANGES_FUNC_BEGIN(minmax)

        /// \brief function template \c minmax
        template(typename T, typename C = less, typename P = identity)(requires indirect_strict_weak_order<C, projected<T const *, P>>)
        constexpr minmax_result<T const &> EARANGES_FUNC(minmax)(T const & a, T const & b, C pred = C{}, P proj = P{}) //
        {
            using R = minmax_result<T const &>;
            return invoke(pred, invoke(proj, b), invoke(proj, a)) ? R{b, a} : R{a, b};
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(
            requires input_range<Rng> AND
            indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>> AND
            indirectly_copyable_storable<iterator_t<Rng>, range_value_t<Rng> *>)
        constexpr minmax_result<range_value_t<Rng>> //
        EARANGES_FUNC(minmax)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            using R = minmax_result<range_value_t<Rng>>;
            auto first = ranges::begin(rng);
            auto last = ranges::end(rng);
            EARANGES_EXPECT(first != last);
            auto result = R{*first, *first};
            if(++first != last)
            {
                {
                    auto && tmp = *first;
                    if(invoke(pred, invoke(proj, tmp), invoke(proj, result.min)))
                        result.min = (decltype(tmp) &&)tmp;
                    else
                        result.max = (decltype(tmp) &&)tmp;
                }
                while(++first != last)
                {
                    range_value_t<Rng> tmp1 = *first;
                    if(++first == last)
                    {
                        if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.min)))
                            result.min = eastl::move(tmp1);
                        else if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.max)))
                            result.max = eastl::move(tmp1);
                        break;
                    }

                    auto && tmp2 = *first;
                    if(invoke(pred, invoke(proj, tmp2), invoke(proj, tmp1)))
                    {
                        if(invoke(pred, invoke(proj, tmp2), invoke(proj, result.min)))
                            result.min = (decltype(tmp2) &&)tmp2;
                        if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.max)))
                            result.max = eastl::move(tmp1);
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.min)))
                            result.min = eastl::move(tmp1);
                        if(!invoke(pred, invoke(proj, tmp2), invoke(proj, result.max)))
                            result.max = (decltype(tmp2) &&)tmp2;
                    }
                }
            }
            return result;
        }

        /// \overload
        template(typename T, typename C = less, typename P = identity)(requires copyable<T> AND indirect_strict_weak_order<C, projected<T const *, P>>)
        constexpr minmax_result<T> EARANGES_FUNC(minmax)(std::initializer_list<T> const && rng, C pred = C{}, P proj = P{}) //
        {
            return (*this)(rng, eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(minmax)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif