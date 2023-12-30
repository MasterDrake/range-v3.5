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

#ifndef EARANGES_ACTION_SPLIT_HPP
#define EARANGES_ACTION_SPLIT_HPP

#include <EASTL/vector.h>


#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/concepts.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/conversion.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/split.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct split_fn
        {
            template<typename Rng>
            using split_value_t =
                meta::if_c<(bool)ranges::container<Rng>, //
                           uncvref_t<Rng>, eastl::vector<range_value_t<Rng>>>;

            template(typename T)(requires range<T &>)
            constexpr auto operator()(T & t) const
            {
                return make_action_closure(bind_back(split_fn{}, detail::reference_wrapper_<T>(t)));
            }

            template<typename T>
            constexpr auto operator()(T && t) const
            {
                return make_action_closure(bind_back(split_fn{}, static_cast<T &&>(t)));
            }

            // BUGBUG something is not right with the actions. It should be possible
            // to move a container into a split and have elements moved into the result.
            template(typename Rng)(requires input_range<Rng> AND indirectly_comparable<iterator_t<Rng>, range_value_t<Rng> const *, ranges::equal_to>)
            eastl::vector<split_value_t<Rng>> //
            operator()(Rng && rng, range_value_t<Rng> val) const
            {
                return views::split(rng, eastl::move(val)) | to<eastl::vector<split_value_t<Rng>>>();
            }

            template(typename Rng, typename Pattern)(
                requires input_range<Rng> AND viewable_range<Pattern> AND
                    forward_range<Pattern> AND
                    indirectly_comparable<
                        iterator_t<Rng>,
                        iterator_t<Pattern>,
                        ranges::equal_to> AND
                    (forward_range<Rng> || detail::tiny_range<Pattern>)) //
            eastl::vector<split_value_t<Rng>> operator()(Rng && rng, Pattern && pattern)
                const
            {
                return views::split(rng, static_cast<Pattern &&>(pattern)) | to<eastl::vector<split_value_t<Rng>>>();
            }

            /// \cond
            template<typename Rng, typename T>
            invoke_result_t<split_fn, Rng, T &> //
            operator()(Rng && rng, detail::reference_wrapper_<T> r) const
            {
                return (*this)(static_cast<Rng &&>(rng), r.get());
            }
            /// \endcond
        };

        /// \relates actions::split_fn
        EARANGES_INLINE_VARIABLE(split_fn, split)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif