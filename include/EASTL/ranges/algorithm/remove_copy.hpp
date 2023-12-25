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
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_HPP

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
    using remove_copy_result = detail::in_out_result<I, O>;

    RANGES_FUNC_BEGIN(remove_copy)

        /// \brief function template \c remove_copy
        template(typename I, typename S, typename O, typename T, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                weakly_incrementable<O> AND
                indirect_relation<equal_to, projected<I, P>, T const *> AND
                indirectly_copyable<I, O>)
        constexpr remove_copy_result<I, O> RANGES_FUNC(remove_copy)(
            I first, S last, O out, T const & val, P proj = P{}) //
        {
            for(; first != last; ++first)
            {
                auto && x = *first;
                if(!(invoke(proj, x) == val))
                {
                    *out = (decltype(x) &&)x;
                    ++out;
                }
            }
            return {first, out};
        }

        /// \overload
        template(typename Rng, typename O, typename T, typename P = identity)(
            requires input_range<Rng> AND weakly_incrementable<O> AND
            indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T const *> AND
            indirectly_copyable<iterator_t<Rng>, O>)
        constexpr remove_copy_result<borrowed_iterator_t<Rng>, O> //
        RANGES_FUNC(remove_copy)(Rng && rng, O out, T const & val, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out), val, eastl::move(proj));
        }

    RANGES_FUNC_END(remove_copy)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif