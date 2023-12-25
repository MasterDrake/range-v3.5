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
#ifndef RANGES_V3_ALGORITHM_GENERATE_HPP
#define RANGES_V3_ALGORITHM_GENERATE_HPP

#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/invoke.hpp"
#include "../functional/reference_wrapper.hpp"
#include "../iterator/concepts.hpp"
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
    template<typename O, typename F>
    using generate_result = detail::out_fun_result<O, F>;

    RANGES_FUNC_BEGIN(generate)

        /// \brief function template \c generate_n
        template(typename O, typename S, typename F)(requires invocable<F &> AND output_iterator<O, invoke_result_t<F &>> AND sentinel_for<S, O>)
        constexpr generate_result<O, F> RANGES_FUNC(generate)(O first, S last, F fun) //
        {
            for(; first != last; ++first)
                *first = invoke(fun);
            return {detail::move(first), detail::move(fun)};
        }

        /// \overload
        template(typename Rng, typename F)(requires invocable<F &> AND output_range<Rng, invoke_result_t<F &>>)
        constexpr generate_result<borrowed_iterator_t<Rng>, F> //
        RANGES_FUNC(generate)(Rng && rng, F fun)
        {
            return {(*this)(begin(rng), end(rng), ref(fun)).out, detail::move(fun)};
        }

    RANGES_FUNC_END(generate)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif