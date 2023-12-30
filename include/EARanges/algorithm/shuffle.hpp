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
//TODO: aggiornare eastl con random e togliere questo
#include <random>
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
        I EARANGES_FUNC(shuffle)(I const first, S const last, Gen && gen = detail::get_random_engine()) //
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
                EARANGES_ENSURE(mid - first <= PTRDIFF_MAX);
                if(auto const i = uid(gen, param_t{0, D2(mid - first)}))
                    ranges::iter_swap(mid - i, mid);
            }
            return mid;
        }

        /// \overload
        template(typename Rng, typename Gen = detail::default_random_engine &)(requires random_access_range<Rng> AND permutable<iterator_t<Rng>> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, iter_difference_t<iterator_t<Rng>>>)
        borrowed_iterator_t<Rng> //
        EARANGES_FUNC(shuffle)(Rng && rng, Gen && rand = detail::get_random_engine()) //
        {
            return (*this)(begin(rng), end(rng), static_cast<Gen &&>(rand));
        }

    EARANGES_FUNC_END(shuffle)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif