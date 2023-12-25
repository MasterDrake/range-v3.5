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
#ifndef RANGES_V3_ALGORITHM_ROTATE_COPY_HPP
#define RANGES_V3_ALGORITHM_ROTATE_COPY_HPP

#include "EASTL/functional.h"

#include "../range_fwd.hpp"

#include "../algorithm/copy.hpp"
#include "../algorithm/result_types.hpp"
#include "../functional/identity.hpp"
#include "../iterator/concepts.hpp"
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
    template<typename I, typename O>
    using rotate_copy_result = detail::in_out_result<I, O>;

    RANGES_FUNC_BEGIN(rotate_copy)

        /// \brief function template \c rotate_copy
        template(typename I, typename S, typename O, typename P = identity)(requires forward_iterator<I> AND sentinel_for<S, I> AND weakly_incrementable<O> AND indirectly_copyable<I, O>)
        constexpr rotate_copy_result<I, O> //
        RANGES_FUNC(rotate_copy)(I first, I middle, S last, O out) //
        {
            auto res = ranges::copy(middle, eastl::move(last), eastl::move(out));
            return {eastl::move(res.in), ranges::copy(eastl::move(first), middle, eastl::move(res.out)).out};
        }

        /// \overload
        template(typename Rng, typename O, typename P = identity)(requires range<Rng> AND weakly_incrementable<O> AND indirectly_copyable<iterator_t<Rng>, O>)
        constexpr rotate_copy_result<borrowed_iterator_t<Rng>, O> //
        RANGES_FUNC(rotate_copy)(Rng && rng, iterator_t<Rng> middle, O out) //
        {
            return (*this)(begin(rng), eastl::move(middle), end(rng), eastl::move(out));
        }

    RANGES_FUNC_END(rotate_copy)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif