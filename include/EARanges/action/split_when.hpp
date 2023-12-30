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

#ifndef EARANGES_ACTION_SPLIT_WHEN_HPP
#define EARANGES_ACTION_SPLIT_WHEN_HPP

#include <EASTL/vector.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/concepts.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/conversion.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/split_when.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct split_when_fn
        {
            template<typename Rng>
            using split_value_t =
                meta::if_c<(bool)ranges::container<Rng>, //
                           uncvref_t<Rng>, eastl::vector<range_value_t<Rng>>>;

            template<typename Fun>
            constexpr auto operator()(Fun fun) const
            {
                return make_action_closure(bind_back(split_when_fn{}, static_cast<Fun &&>(fun)));
            }

            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            template(typename Rng, typename Fun)(
                requires forward_range<Rng> AND
                        invocable<Fun &, iterator_t<Rng>, sentinel_t<Rng>> AND
                            invocable<Fun &, iterator_t<Rng>, iterator_t<Rng>> AND
                                copy_constructible<Fun> AND
                                    convertible_to<invoke_result_t<Fun &, iterator_t<Rng>,
                                                                   sentinel_t<Rng>>,
                                                   eastl::pair<bool, iterator_t<Rng>>>)
            eastl::vector<split_value_t<Rng>> operator()(Rng && rng, Fun fun) const
            {
                return views::split_when(rng, eastl::move(fun)) | to<eastl::vector<split_value_t<Rng>>>();
            }

            template(typename Rng, typename Fun)(
                requires forward_range<Rng> AND
                        predicate<Fun const &, range_reference_t<Rng>> AND
                            copy_constructible<Fun>) eastl::vector<split_value_t<Rng>>
            operator()(Rng && rng, Fun fun) const
            {
                return views::split_when(rng, eastl::move(fun)) | to<eastl::vector<split_value_t<Rng>>>();
            }
        };

        /// \relates actions::split_when_fn
        EARANGES_INLINE_VARIABLE(split_when_fn, split_when)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif