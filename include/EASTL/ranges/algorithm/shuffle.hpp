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
#ifndef RANGES_V3_ALGORITHM_SHUFFLE_HPP
#define RANGES_V3_ALGORITHM_SHUFFLE_HPP

#include <cstdint>
#include <random>
//#include <EASTL/utility.h>
#include "EASTL/utility.h"
#include "EASTL/random.h"

#include "../range_fwd.hpp"

#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/dangling.hpp"
#include "../range/traits.hpp"
#include "../utility/random.hpp"
#include "../utility/static_const.hpp"
#include "../utility/swap.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(shuffle)

        /// \brief function template \c shuffle
        template(typename I, typename S, typename Gen = detail::default_random_engine &)(requires random_access_iterator<I> AND sentinel_for<S, I> AND permutable<I> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, iter_difference_t<I>>)
        I RANGES_FUNC(shuffle)(I const first, S const last, Gen && gen = detail::get_random_engine()) //
        {
            auto mid = first;
            if(mid == last)
                return mid;
            using D1 = iter_difference_t<I>;
            using D2 = meta::conditional_t<eastl::is_integral<D1>::value, D1, std::ptrdiff_t>;
            //TODO:Upgrade eastl std::unirofm_int_distribution because right now it doesn't support long I guess
            std::uniform_int_distribution<D2> uid{};
            using param_t = typename decltype(uid)::param_type;
            while(++mid != last)
            {
                RANGES_ENSURE(mid - first <= PTRDIFF_MAX);
                if(auto const i = uid(gen, param_t{0, D2(mid - first)}))
                    ranges::iter_swap(mid - i, mid);
            }
            return mid;
        }

        /// \overload
        template(typename Rng, typename Gen = detail::default_random_engine &)(requires random_access_range<Rng> AND permutable<iterator_t<Rng>> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, iter_difference_t<iterator_t<Rng>>>)
        borrowed_iterator_t<Rng> //
        RANGES_FUNC(shuffle)(Rng && rng, Gen && rand = detail::get_random_engine()) //
        {
            return (*this)(begin(rng), end(rng), static_cast<Gen &&>(rand));
        }

    RANGES_FUNC_END(shuffle)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif