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
#ifndef EARANGES_ALGORITHM_REPLACE_HPP
#define EARANGES_ALGORITHM_REPLACE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

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
    EARANGES_FUNC_BEGIN(replace)

        /// \brief function template \c replace
        template(typename I, typename S, typename T1, typename T2, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                indirectly_writable<I, T2 const &> AND
                indirect_relation<equal_to, projected<I, P>, T1 const *>)
        constexpr I EARANGES_FUNC(replace)(I first, S last, T1 const & old_value, T2 const & new_value, P proj = {}) //
        {
            for(; first != last; ++first)
                if(invoke(proj, *first) == old_value)
                    *first = new_value;
            return first;
        }

        /// \overload
        template(typename Rng, typename T1, typename T2, typename P = identity)(
            requires input_range<Rng> AND
                indirectly_writable<iterator_t<Rng>, T2 const &> AND
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T1 const *>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(replace)(Rng && rng, T1 const & old_value, T2 const & new_value, P proj = {}) //
        {
            return (*this)(begin(rng), end(rng), old_value, new_value, eastl::move(proj));
        }

    EARANGES_FUNC_END(replace)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif