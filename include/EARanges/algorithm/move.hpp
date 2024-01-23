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
#ifndef EARANGES_ALGORITHM_MOVE_HPP
#define EARANGES_ALGORITHM_MOVE_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/move.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using move_result = detail::in_out_result<I, O>;

    EARANGES_HIDDEN_DETAIL(namespace _move CPP_PP_LBRACE())
    EARANGES_FUNC_BEGIN(move)

        /// \brief function template \c move
        template(typename I, typename S, typename O)(requires input_iterator<I> AND sentinel_for<S, I> AND weakly_incrementable<O> AND indirectly_movable<I, O>)
        constexpr move_result<I, O> EARANGES_FUNC(move)(I first, S last, O out) //
        {
            for(; first != last; ++first, ++out)
                *out = iter_move(first);
            return {first, out};
        }

        /// \overload
        template(typename Rng, typename O)(requires input_range<Rng> AND weakly_incrementable<O> AND indirectly_movable<iterator_t<Rng>, O>)
        constexpr move_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(move)(Rng && rng, O out)            //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out));
        }

    EARANGES_FUNC_END(move)
    EARANGES_HIDDEN_DETAIL(CPP_PP_RBRACE())

#ifndef EARANGES_DOXYGEN_INVOKED
    struct EARANGES_EMPTY_BASES move_fn
      : aux::move_fn
      , _move::move_fn
    {
        using aux::move_fn::operator();
        using _move::move_fn::operator();
    };

    EARANGES_INLINE_VARIABLE(move_fn, move)
#endif

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
