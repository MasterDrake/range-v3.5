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
#ifndef EARANGES_ALGORITHM_REVERSE_COPY_HPP
#define EARANGES_ALGORITHM_REVERSE_COPY_HPP

#include <EARanges/meta/meta.hpp>

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
    using reverse_copy_result = detail::in_out_result<I, O>;

    EARANGES_FUNC_BEGIN(reverse_copy)

        /// \brief function template \c reverse_copy
        template(typename I, typename S, typename O)(requires bidirectional_iterator<I> AND sentinel_for<S, I> AND weakly_incrementable<O> AND indirectly_copyable<I, O>)
        constexpr reverse_copy_result<I, O> EARANGES_FUNC(reverse_copy)(I first, S end_, O out) //
        {
            I last = ranges::next(first, end_), res = last;
            for(; first != last; ++out)
                *out = *--last;
            return {res, out};
        }

        /// \overload
        template(typename Rng, typename O)(requires bidirectional_range<Rng> AND weakly_incrementable<O> AND indirectly_copyable<iterator_t<Rng>, O>)
        constexpr reverse_copy_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(reverse_copy)(Rng && rng, O out)            //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out));
        }

    EARANGES_FUNC_END(reverse_copy)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif