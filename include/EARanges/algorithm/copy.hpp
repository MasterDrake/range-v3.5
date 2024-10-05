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
#ifndef EARANGES_ALGORITHM_COPY_HPP
#define EARANGES_ALGORITHM_COPY_HPP

#include <EASTL/functional.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/copy.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using copy_result = detail::in_out_result<I, O>;

    EARANGES_HIDDEN_DETAIL(namespace _copy CPP_PP_LBRACE())
    EARANGES_FUNC_BEGIN(copy)

        /// \brief function template \c copy
        template(typename I, typename S, typename O)(requires input_iterator<I> AND sentinel_for<S, I> AND weakly_incrementable<O> AND indirectly_copyable<I, O>)
        constexpr copy_result<I, O> EARANGES_FUNC(copy)(I first, S last, O out) //
        {
            return eastl::copy(first, last, out);
        }

        /// \overload
        template(typename Rng, typename O)(requires input_range<Rng> AND weakly_incrementable<O> AND indirectly_copyable<iterator_t<Rng>, O>)
        constexpr copy_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(copy)(Rng && rng, O out)  //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out));
        }

    EARANGES_FUNC_END(copy)
    EARANGES_HIDDEN_DETAIL(CPP_PP_RBRACE())

#ifndef EARANGES_DOXYGEN_INVOKED
    struct copy_fn
      : aux::copy_fn
      , _copy::copy_fn
    {
        using aux::copy_fn::operator();
        using _copy::copy_fn::operator();
    };
    EARANGES_INLINE_VARIABLE(copy_fn, copy)
#endif

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
