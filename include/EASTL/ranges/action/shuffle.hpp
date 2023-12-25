/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_SHUFFLE_HPP
#define RANGES_V3_ACTION_SHUFFLE_HPP

#include "../range_fwd.hpp"

#include "../action/action.hpp"
#include "../algorithm/shuffle.hpp"
#include "../functional/bind.hpp"
#include "../functional/bind_back.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct shuffle_fn
        {
            template(typename Gen)(requires uniform_random_bit_generator<Gen>)
            constexpr auto operator()(Gen & gen) const
            {
                return make_action_closure(bind_back(shuffle_fn{}, detail::reference_wrapper_<Gen>(gen)));
            }

            template(typename Gen)(requires uniform_random_bit_generator<Gen>)
            constexpr auto operator()(Gen && gen) const
            {
                return make_action_closure(bind_back(shuffle_fn{}, static_cast<Gen &&>(gen)));
            }

            template(typename Rng, typename Gen)(requires random_access_range<Rng> AND permutable<iterator_t<Rng>> AND uniform_random_bit_generator<eastl::remove_reference_t<Gen>> AND convertible_to<invoke_result_t<Gen &>, range_difference_t<Rng>>)
            Rng operator()(Rng && rng, Gen && gen) const
            {
                ranges::shuffle(rng, static_cast<Gen &&>(gen));
                return static_cast<Rng &&>(rng);
            }

            /// \cond
            template<typename Rng, typename T>
            invoke_result_t<shuffle_fn, Rng, T &> //
            operator()(Rng && rng, detail::reference_wrapper_<T> r) const
            {
                return (*this)(static_cast<Rng &&>(rng), r.get());
            }
            /// \endcond
        };

        /// \relates actions::shuffle_fn
        /// \sa `action_closure`
        RANGES_INLINE_VARIABLE(shuffle_fn, shuffle)
    } // namespace actions
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif