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
#ifndef EARANGES_ALGORITHM_MOVE_BACKWARD_HPP
#define EARANGES_ALGORITHM_MOVE_BACKWARD_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
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
    template<typename I, typename O>
    using move_backward_result = detail::in_out_result<I, O>;

    EARANGES_FUNC_BEGIN(move_backward)

        /// \brief function template \c move_backward
        template(typename I, typename S, typename O)(requires bidirectional_iterator<I> AND sentinel_for<S, I> AND bidirectional_iterator<O> AND indirectly_movable<I, O>)
        constexpr move_backward_result<I, O> EARANGES_FUNC(move_backward)(I first, S end_, O out) //
        {
            I i = ranges::next(first, end_), last = i;
            while(first != i)
                *--out = iter_move(--i);
            return {last, out};
        }

        /// \overload
        template(typename Rng, typename O)(requires bidirectional_range<Rng> AND bidirectional_iterator<O> AND indirectly_movable<iterator_t<Rng>, O>)
        constexpr move_backward_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(move_backward)(Rng && rng, O out)            //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out));
        }

    EARANGES_FUNC_END(move_backward)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif