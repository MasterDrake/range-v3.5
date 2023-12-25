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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_HPP

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
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
    using replace_copy_result = detail::in_out_result<I, O>;

    RANGES_FUNC_BEGIN(replace_copy)

        /// \brief function template \c replace_copy
        template(typename I,
                 typename S,
                 typename O,
                 typename T1,
                 typename T2,
                 typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                output_iterator<O, T2 const &> AND indirectly_copyable<I, O> AND
                indirect_relation<equal_to, projected<I, P>, T1 const *>)
        constexpr replace_copy_result<I, O> RANGES_FUNC(replace_copy)(I first,
                                                                      S last,
                                                                      O out,
                                                                      T1 const & old_value,
                                                                      T2 const & new_value,
                                                                      P proj = {}) //
        {
            for(; first != last; ++first, ++out)
            {
                auto && x = *first;
                if(invoke(proj, x) == old_value)
                    *out = new_value;
                else
                    *out = (decltype(x) &&)x;
            }
            return {first, out};
        }

        /// \overload
        template(typename Rng,
                 typename O,
                 typename T1,
                 typename T2,
                 typename P = identity)(
            requires input_range<Rng> AND output_iterator<O, T2 const &> AND
                indirectly_copyable<iterator_t<Rng>, O> AND
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T1 const *>)
        constexpr replace_copy_result<borrowed_iterator_t<Rng>, O> RANGES_FUNC(replace_copy)(
            Rng && rng, O out, T1 const & old_value, T2 const & new_value, P proj = {}) //
        {
            return (*this)(begin(rng),
                           end(rng),
                           eastl::move(out),
                           old_value,
                           new_value,
                           eastl::move(proj));
        }

    RANGES_FUNC_END(replace_copy)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif