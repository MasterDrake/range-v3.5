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
#ifndef RANGES_V3_ALGORITHM_GENERATE_N_HPP
#define RANGES_V3_ALGORITHM_GENERATE_N_HPP
//TODO: A che cazzo serve???????????????????????
//#include <tuple>
//#include <utility>

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename O, typename F>
    using generate_n_result = detail::out_fun_result<O, F>;

    RANGES_FUNC_BEGIN(generate_n)

        /// \brief function template \c generate_n
        template(typename O, typename F)(requires invocable<F &> AND output_iterator<O, invoke_result_t<F &>>)
        constexpr generate_n_result<O, F> //
        RANGES_FUNC(generate_n)(O first, iter_difference_t<O> n, F fun)
        {
            RANGES_EXPECT(n >= 0);
            auto norig = n;
            auto b = uncounted(first);
            for(; 0 != n; ++b, --n)
                *b = invoke(fun);
            return {recounted(first, b, norig), detail::move(fun)};
        }

    RANGES_FUNC_END(generate_n)

    // @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif