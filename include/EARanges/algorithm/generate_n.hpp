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
#ifndef EARANGES_ALGORITHM_GENERATE_N_HPP
#define EARANGES_ALGORITHM_GENERATE_N_HPP

#include <EASTL/tuple.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename O, typename F>
    using generate_n_result = detail::out_fun_result<O, F>;

    EARANGES_FUNC_BEGIN(generate_n)

        /// \brief function template \c generate_n
        template(typename O, typename F)(requires invocable<F &> AND output_iterator<O, invoke_result_t<F &>>)
        constexpr generate_n_result<O, F> //
        EARANGES_FUNC(generate_n)(O first, iter_difference_t<O> n, F fun)
        {
            return eastl::generate_n(first, n, eastl::move(fun));
        }

    EARANGES_FUNC_END(generate_n)

    // @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif