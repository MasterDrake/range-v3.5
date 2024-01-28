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
#ifndef EARANGES_ALGORITHM_REPLACE_COPY_IF_HPP
#define EARANGES_ALGORITHM_REPLACE_COPY_IF_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        template<typename I, typename O>
        using replace_copy_if_result = detail::in_out_result<I, O>;

        EARANGES_FUNC_BEGIN(replace_copy_if)

            /// \brief function template \c replace_copy_if
            template(typename I,
                     typename S,
                     typename O,
                     typename C,
                     typename T,
                     typename P = identity)(
                requires input_iterator<I> AND sentinel_for<S, I> AND
                    output_iterator<O, T const &>
                        AND indirect_unary_predicate<C, projected<I, P>>
                            AND indirectly_copyable<
                                I,
                                O>) constexpr replace_copy_if_result<I, O>
            EARANGES_FUNC(replace_copy_if)(
                I first, S last, O out, C pred, T const & new_value, P proj = {}) //
            {
                for(; first != last; ++first, ++out)
                {
                    auto && x = *first;
                    if(invoke(pred, invoke(proj, x)))
                        *out = new_value;
                    else
                        *out = (decltype(x) &&)x;
                }
                return {first, out};
            }

            /// \overload
            template(
                typename Rng, typename O, typename C, typename T, typename P = identity)(
                requires input_range<Rng> AND output_iterator<O, T const &> AND
                    indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>
                        AND indirectly_copyable<
                            iterator_t<Rng>,
                            O>) constexpr replace_copy_if_result<borrowed_iterator_t<Rng>,
                                                                 O>
            EARANGES_FUNC(replace_copy_if)(
                Rng && rng, O out, C pred, T const & new_value, P proj = {}) //
            {
                return (*this)(begin(rng),
                               end(rng),
                               eastl::move(out),
                               eastl::move(pred),
                               new_value,
                               eastl::move(proj));
            }

        EARANGES_FUNC_END(replace_copy_if)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif