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
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

//#include <EASTL/functional.h>
#include "EASTL/functional.h"

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../functional/reference_wrapper.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/dangling.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename F>
    using for_each_result = detail::in_fun_result<I, F>;

    RANGES_FUNC_BEGIN(for_each)

        /// \brief function template \c for_each
        template(typename I, typename S, typename F, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirectly_unary_invocable<F, projected<I, P>>)
        constexpr for_each_result<I, F> RANGES_FUNC(for_each)(I first, S last, F fun, P proj = P{})
        {
            for(; first != last; ++first)
            {
                invoke(fun, invoke(proj, *first));
            }
            return {detail::move(first), detail::move(fun)};
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirectly_unary_invocable<F, projected<iterator_t<Rng>, P>>)
        constexpr for_each_result<borrowed_iterator_t<Rng>, F> //
        RANGES_FUNC(for_each)(Rng && rng, F fun, P proj = P{})
        {
            return {(*this)(begin(rng), end(rng), ref(fun), detail::move(proj)).in, detail::move(fun)};
        }

    RANGES_FUNC_END(for_each)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif