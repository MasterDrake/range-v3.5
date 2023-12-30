/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_SAMPLE_HPP
#define EARANGES_ALGORITHM_SAMPLE_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/copy_n.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
//TODO: Use eastl::uniform_int_distribution in random instead of std::, check this header
#include <EARanges/utility/random.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using sample_result = detail::in_out_result<I, O>;

    /// \cond
    namespace detail
    {
        template<typename I, typename S, typename O, typename Gen>
        sample_result<I, O> sample_sized_impl(I first,
                                              S last,
                                              iter_difference_t<I> pop_size,
                                              O out,
                                              iter_difference_t<O> sample_size,
                                              Gen && gen)
        {
            if(pop_size > 0 && sample_size > 0)
            {
                eastl::uniform_int_distribution<iter_difference_t<I>> dist;
                using param_t = typename decltype(dist)::param_type;
                for(; first != last; ++first)
                {
                    if(sample_size >= pop_size)
                        return copy_n(eastl::move(first), pop_size, eastl::move(out));

                    if(dist(gen, param_t{0, --pop_size}) < sample_size)
                    {
                        *out = *first;
                        ++out;
                        if(--sample_size == 0)
                            break;
                    }
                }
            }

            return {eastl::move(first), eastl::move(out)};
        }
    } // namespace detail
    /// \endcond

    EARANGES_FUNC_BEGIN(sample)

        /// \brief function template \c sample
        template(typename I,
                 typename S,
                 typename O,
                 typename Gen = detail::default_random_engine &)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                weakly_incrementable<O> AND indirectly_copyable<I, O> AND
                uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND
                (random_access_iterator<O> || forward_iterator<I> ||
                 sized_sentinel_for<S, I>))
        sample_result<I, O> EARANGES_FUNC(sample)(I first,
                                                S last,
                                                O out,
                                                iter_difference_t<O> const n,
                                                Gen && gen = detail::get_random_engine())
        {
            if(EARANGES_CONSTEXPR_IF(forward_iterator<I> || sized_sentinel_for<S, I>)) //
            {
                auto const k = distance(first, last);
                return detail::sample_sized_impl(eastl::move(first),
                                                 eastl::move(last),
                                                 k,
                                                 eastl::move(out),
                                                 n,
                                                 static_cast<Gen &&>(gen));
            }
            else
            {
                // out is random-access here; calls to advance(out,n) and
                // next(out,n) are O(1).
                if(n > 0)
                {
                    for(iter_difference_t<O> i = 0; i < n; (void)++i, ++first)
                    {
                        if(first == last)
                        {
                            advance(out, i);
                            goto done;
                        }
                        *next(out, i) = *first;
                    }

                    eastl::uniform_int_distribution<iter_difference_t<O>> dist;
                    using param_t = typename decltype(dist)::param_type;
                    for(auto pop_size = n; first != last; (void)++first, ++pop_size)
                    {
                        auto const i = dist(gen, param_t{0, pop_size});
                        if(i < n)
                            *next(out, i) = *first;
                    }

                    advance(out, n);
                }
            done:
                return {eastl::move(first), eastl::move(out)};
            }
        }

        /// \overload
        template(typename I,
                 typename S,
                 typename ORng,
                 typename Gen = detail::default_random_engine &)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                weakly_incrementable<iterator_t<ORng>> AND
                indirectly_copyable<I, iterator_t<ORng>> AND
                uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND
                (forward_range<ORng> || sized_range<ORng>) AND
                (random_access_iterator<iterator_t<ORng>> || forward_iterator<I> ||
                    sized_sentinel_for<S, I>))
        sample_result<I, borrowed_iterator_t<ORng>> EARANGES_FUNC(sample)(
            I first,
            S last,
            ORng && out,
            Gen && gen = detail::get_random_engine()) //
        {
            if(EARANGES_CONSTEXPR_IF(forward_iterator<I> || sized_sentinel_for<S, I>)) //
            {
                auto k = distance(first, last);
                return detail::sample_sized_impl(eastl::move(first),
                                                 eastl::move(last),
                                                 k,
                                                 begin(out),
                                                 distance(out),
                                                 static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(eastl::move(first),
                               eastl::move(last),
                               begin(out),
                               distance(out),
                               static_cast<Gen &&>(gen));
            }
        }

        /// \overload
        template(typename Rng,
                 typename O,
                 typename Gen = detail::default_random_engine &)(
            requires input_range<Rng> AND weakly_incrementable<O> AND
                indirectly_copyable<iterator_t<Rng>, O> AND
                uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND
                (random_access_iterator<O> || forward_range<Rng> || sized_range<Rng>))
        sample_result<borrowed_iterator_t<Rng>, O> EARANGES_FUNC(sample)(
            Rng && rng,
            O out,
            iter_difference_t<O> const n,
            Gen && gen = detail::get_random_engine()) //
        {
            if(EARANGES_CONSTEXPR_IF(forward_range<Rng> || sized_range<Rng>)) //
            {
                return detail::sample_sized_impl(begin(rng),
                                                 end(rng),
                                                 distance(rng),
                                                 eastl::move(out),
                                                 n,
                                                 static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(out), n, static_cast<Gen &&>(gen));
            }
        }

        /// \overload
        template(typename IRng,
                 typename ORng,
                 typename Gen = detail::default_random_engine &)(
            requires input_range<IRng> AND range<ORng> AND
                indirectly_copyable<iterator_t<IRng>, iterator_t<ORng>> AND
                uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND
                (random_access_iterator<iterator_t<ORng>> || forward_range<IRng> ||
                    sized_range<IRng>) AND
                (forward_range<ORng> || sized_range<ORng>))
        sample_result<borrowed_iterator_t<IRng>, borrowed_iterator_t<ORng>> //
        EARANGES_FUNC(sample)(IRng && rng,
                            ORng && out,
                            Gen && gen = detail::get_random_engine())
        {
            if(EARANGES_CONSTEXPR_IF(forward_range<IRng> || sized_range<IRng>)) //
            {
                return detail::sample_sized_impl(begin(rng),
                                                 end(rng),
                                                 distance(rng),
                                                 begin(out),
                                                 distance(out),
                                                 static_cast<Gen &&>(gen));
            }
            else
            {
                return (*this)(begin(rng),
                               end(rng),
                               begin(out),
                               distance(out),
                               static_cast<Gen &&>(gen));
            }
        }

    EARANGES_FUNC_END(sample)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif