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

#ifndef EARANGES_ACTION_SLICE_HPP
#define EARANGES_ACTION_SLICE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/interface.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct slice_fn
        {
        private:
            template<typename D>
            using diff_t = range_difference_t<D>;

        public:
            // Overloads for the pipe syntax: rng | actions::slice(from, to)
            template(typename D)(requires integral<D>)
            constexpr auto operator()(D from, D to) const
            {
                return make_action_closure(bind_back(slice_fn{}, from, to));
            }
            template(typename D)(requires integral<D>)
            constexpr auto operator()(D from, detail::from_end_<D> to) const
            {
                return make_action_closure(bind_back(slice_fn{}, from, to));
            }
            template(typename D)(requires integral<D>)
            constexpr auto operator()(detail::from_end_<D> from, detail::from_end_<D> to) const
            {
                return make_action_closure(bind_back(slice_fn{}, from, to));
            }
            template(typename D)(requires integral<D>)
            constexpr auto operator()(D from, end_fn const & to) const
            {
                return make_action_closure(bind_back(slice_fn{}, from, to));
            }
            template(typename D)(requires integral<D>)
            constexpr auto operator()(detail::from_end_<D> from, end_fn const & to) const
            {
                return make_action_closure(bind_back(slice_fn{}, from, to));
            }

            template(typename Rng, typename I = iterator_t<Rng>)(requires forward_range<Rng> AND erasable_range<Rng &, I, I>)
            Rng operator()(Rng && rng, diff_t<Rng> from, diff_t<Rng> to) const
            {
                EARANGES_EXPECT(0 <= from && 0 <= to && from <= to);
                EARANGES_EXPECT(!sized_range<Rng> || to <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                ranges::actions::erase(rng, next(begin(rng), to - from), end(rng));
                return static_cast<Rng &&>(rng);
            }

            template(typename Rng, typename I = iterator_t<Rng>)(requires bidirectional_range<Rng> AND erasable_range<Rng &, I, I>)
            Rng operator()(Rng && rng, diff_t<Rng> from, detail::from_end_<diff_t<Rng>> to) const
            {
                EARANGES_EXPECT(0 <= from && to.dist_ <= 0);
                EARANGES_EXPECT(!sized_range<Rng> || from - to.dist_ <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                if(to.dist_ != 0)
                {
                    auto const last = next(begin(rng), end(rng));
                    ranges::actions::erase(rng, prev(last, -to.dist_), last);
                }
                return static_cast<Rng &&>(rng);
            }

            template(typename Rng, typename I = iterator_t<Rng>)(requires bidirectional_range<Rng> AND erasable_range<Rng &, I, I>)
            Rng operator()(Rng && rng, detail::from_end_<diff_t<Rng>> from, detail::from_end_<diff_t<Rng>> to) const
            {
                EARANGES_EXPECT(from.dist_ <= 0 && to.dist_ <= 0 && from.dist_ <= to.dist_);
                EARANGES_EXPECT(!sized_range<Rng> || 0 <= distance(rng) + from.dist_);
                auto last = next(begin(rng), end(rng));
                ranges::actions::erase(rng, prev(last, -to.dist_), last);
                last = next(begin(rng), end(rng));
                ranges::actions::erase(rng, begin(rng), prev(last, to.dist_ - from.dist_));
                return static_cast<Rng &&>(rng);
            }

            template(typename Rng, typename I = iterator_t<Rng>)(requires forward_range<Rng> AND erasable_range<Rng &, I, I>)
            Rng operator()(Rng && rng, diff_t<Rng> from, end_fn const &) const
            {
                EARANGES_EXPECT(0 <= from);
                EARANGES_EXPECT(!sized_range<Rng> || from <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                return static_cast<Rng &&>(rng);
            }

            template(typename Rng, typename I = iterator_t<Rng>)(requires bidirectional_range<Rng> AND erasable_range<Rng &, I, I>)
            Rng operator()(Rng && rng, detail::from_end_<diff_t<Rng>> from, end_fn const &) const
            {
                EARANGES_EXPECT(from.dist_ <= 0);
                EARANGES_EXPECT(!sized_range<Rng> || 0 <= distance(rng) + from.dist_);
                auto const last = next(begin(rng), end(rng));
                ranges::actions::erase(rng, begin(rng), prev(last, -from.dist_));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::slice_fn
        EARANGES_INLINE_VARIABLE(slice_fn, slice)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif