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
#ifndef EARANGES_ALGORITHM_SHUFFLE_HPP
#define EARANGES_ALGORITHM_SHUFFLE_HPP

#include <cstdint>

#include <EASTL/random.h>
#include <EASTL/utility.h>
#include <EASTL/random.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/random.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/utility/swap.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(shuffle)

        /// \brief function template \c shuffle
        template(typename I, typename S, typename Gen = detail::default_random_engine &)(requires random_access_iterator<I> AND sentinel_for<S, I> AND permutable<I> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, iter_difference_t<I>>)
        void EARANGES_FUNC(shuffle)(I const first, S const last, Gen && gen = detail::get_random_engine()) //
        {
            if(first != last)
            {
                using diff_type = iter_difference_t<I>;
                using udiff_type = eastl::make_unsigned_t<diff_type>;

                eastl::uniform_int_distribution<udiff_type> uid{};
                using param_t = typename decltype(uid)::param_type;

                for(I i = first + 1; i != last; ++i)
                {
                    ranges::iter_swap(i, first + uid(gen, param_t{0, udiff_type(i - first)}));
                }
            }
        }

        /// \overload
        template(typename Rng, typename Gen = detail::default_random_engine &)(requires random_access_range<Rng> AND permutable<iterator_t<Rng>> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, iter_difference_t<iterator_t<Rng>>>)
        void //
        EARANGES_FUNC(shuffle)(Rng && rng, Gen && rand = detail::get_random_engine()) //
        {
            return (*this)(begin(rng), end(rng), static_cast<Gen &&>(rand));
        }

    EARANGES_FUNC_END(shuffle)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif