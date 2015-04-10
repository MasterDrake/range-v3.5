/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_COUNT_IF_HPP
#define RANGES_V3_ALGORITHM_COUNT_IF_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct count_if_fn
        {
            template<typename I, typename S, typename R, typename P = ident,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() &&
                    IndirectCallablePredicate<R, Project<I, P> >())>
            iterator_difference_t<I>
            operator()(I begin, S end, R pred_, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                iterator_difference_t<I> n = 0;
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        ++n;
                return n;
            }

            template<typename Rng, typename R, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(InputIterable<Rng>() && IndirectCallablePredicate<R, Project<I, P> >())>
            iterator_difference_t<I>
            operator()(Rng &&rng, R pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `count_if_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& count_if = static_const<with_braced_init_args<count_if_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
