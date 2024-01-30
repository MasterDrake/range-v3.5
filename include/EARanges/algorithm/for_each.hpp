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
#ifndef EARANGES_ALGORITHM_FOR_EACH_HPP
#define EARANGES_ALGORITHM_FOR_EACH_HPP

#include <EASTL/functional.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/functional/reference_wrapper.hpp>
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
    template<typename I, typename F>
    using for_each_result = detail::in_fun_result<I, F>;

    EARANGES_FUNC_BEGIN(for_each)

        /// \brief function template \c for_each
        template(typename I, typename S, typename F, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirectly_unary_invocable<F, projected<I, P>>)
        constexpr for_each_result<I, F> EARANGES_FUNC(for_each)(I first, S last, F fun, P proj = P{})
        {
            for(; first != last; ++first)
                invoke(fun, invoke(proj, *first));

            return {detail::move(first), detail::move(fun)};
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirectly_unary_invocable<F, projected<iterator_t<Rng>, P>>)
        constexpr for_each_result<borrowed_iterator_t<Rng>, F> //
        EARANGES_FUNC(for_each)(Rng && rng, F fun, P proj = P{})
        {
            return {(*this)(begin(rng), end(rng), ref(fun), detail::move(proj)).in, detail::move(fun)};
        }

    EARANGES_FUNC_END(for_each)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
