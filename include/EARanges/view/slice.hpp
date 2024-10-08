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

#ifndef EARANGES_VIEW_SLICE_HPP
#define EARANGES_VIEW_SLICE_HPP


#include <EASTL/type_traits.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/counted_iterator.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/drop_exactly.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/subrange.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Rng, typename Int>
        iterator_t<Rng> pos_at_(Rng && rng, Int i, input_range_tag, eastl::true_type)
        {
            EARANGES_EXPECT(0 <= i);
            return next(ranges::begin(rng), i);
        }

        template<typename Rng, typename Int>
        iterator_t<Rng> pos_at_(Rng && rng, Int i, bidirectional_range_tag, eastl::false_type)
        {
            if(0 > i)
            {
                // If it's not common and we know the size, faster to count from the front
                if(EARANGES_CONSTEXPR_IF(sized_range<Rng> && !common_range<Rng>))
                    return next(ranges::begin(rng), distance(rng) + i);
                // Otherwise, probably faster to count from the back.
                return next(ranges::next(ranges::begin(rng), ranges::end(rng)), i);
            }
            return next(ranges::begin(rng), i);
        }

        template<typename Rng, typename Int>
        iterator_t<Rng> pos_at_(Rng && rng, Int i, input_range_tag, eastl::false_type)
        {
            EARANGES_EXPECT(i >= 0 || (bool)sized_range<Rng> || (bool)forward_range<Rng>);
            if(0 > i)
                return next(ranges::begin(rng), distance(rng) + i);
            return next(ranges::begin(rng), i);
        }

        template<typename Rng, bool IsRandomAccess>
        struct slice_view_ : view_facade<slice_view<Rng>, finite>
        {
        private:
            friend range_access;
            Rng rng_;
            range_difference_t<Rng> from_, count_;
            detail::non_propagating_cache<iterator_t<Rng>> begin_;

            iterator_t<Rng> get_begin_()
            {
                if(!begin_)
                    begin_ = detail::pos_at_(rng_, from_, range_tag_of<Rng>{}, is_infinite<Rng>{});
                return *begin_;
            }

        public:
            slice_view_() = default;
            constexpr slice_view_(Rng rng, range_difference_t<Rng> from, range_difference_t<Rng> count)
              : rng_(eastl::move(rng))
              , from_(from)
              , count_(count)
            {}
            counted_iterator<iterator_t<Rng>> begin()
            {
                return make_counted_iterator(get_begin_(), count_);
            }
            default_sentinel_t end()
            {
                return {};
            }
            auto size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(count_);
            }
            Rng base() const
            {
                return rng_;
            }
        };

        template<typename Rng>
        struct slice_view_<Rng, true> : view_interface<slice_view<Rng>, finite>
        {
        private:
            Rng rng_;
            range_difference_t<Rng> from_, count_;

        public:
            slice_view_() = default;
            constexpr slice_view_(Rng rng, range_difference_t<Rng> from,
                                  range_difference_t<Rng> count)
              : rng_(eastl::move(rng))
              , from_(from)
              , count_(count)
            {
                EARANGES_EXPECT(0 <= count_);
            }
            iterator_t<Rng> begin()
            {
                return detail::pos_at_(rng_, from_, range_tag_of<Rng>{}, is_infinite<Rng>{});
            }
            iterator_t<Rng> end()
            {
                return detail::pos_at_(rng_, from_, range_tag_of<Rng>{}, is_infinite<Rng>{}) + count_;
            }
            template(typename BaseRng = Rng)(requires range<BaseRng const>)
            iterator_t<BaseRng const> begin() const
            {
                return detail::pos_at_(rng_, from_, range_tag_of<Rng>{}, is_infinite<Rng>{});
            }
            template(typename BaseRng = Rng)(requires range<BaseRng const>)
            iterator_t<BaseRng const> end() const
            {
                return detail::pos_at_(rng_, from_, range_tag_of<Rng>{}, is_infinite<Rng>{}) + count_;
            }
            auto size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(count_);
            }
            Rng base() const
            {
                return rng_;
            }
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct slice_view : detail::slice_view_<Rng, (bool)random_access_range<Rng>>
    {
        using detail::slice_view_<Rng, (bool)random_access_range<Rng>>::slice_view_;
    };

    template<typename Rng>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<slice_view<Rng>> = //
        enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    slice_view(Rng &&, range_difference_t<Rng>, range_difference_t<Rng>) -> slice_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct slice_base_fn
        {
        private:
            template<typename Rng>
            static constexpr slice_view<all_t<Rng>> impl_(Rng && rng,
                                                          range_difference_t<Rng> from,
                                                          range_difference_t<Rng> count,
                                                          input_range_tag, range_tag = {})
            {
                return {all(static_cast<Rng &&>(rng)), from, count};
            }
            template(typename Rng)(requires borrowed_range<Rng>)
            static subrange<iterator_t<Rng>> impl_(Rng && rng,
                                                   range_difference_t<Rng> from,
                                                   range_difference_t<Rng> count,
                                                   random_access_range_tag,
                                                   common_range_tag = {})
            {
                auto it = detail::pos_at_(rng, from, range_tag_of<Rng>{}, is_infinite<Rng>{});
                return {it, it + count};
            }

        public:
            // slice(rng, 2, 4)
            template(typename Rng)(requires viewable_range<Rng> AND input_range<Rng>)
            constexpr auto operator()(Rng && rng,
                                      range_difference_t<Rng> from,
                                      range_difference_t<Rng> to) const
            {
                EARANGES_EXPECT(0 <= from && from <= to);
                return slice_base_fn::impl_(static_cast<Rng &&>(rng), from, to - from, range_tag_of<Rng>{});
            }
            // slice(rng, 4, end-2)
            //  TODO Support Forward, non-Sized ranges by returning a range that doesn't know it's size? - Eric Niebler
            template(typename Rng)(requires viewable_range<Rng> AND input_range<Rng> AND sized_range<Rng>)
            auto operator()(Rng && rng,
                            range_difference_t<Rng> from,
                            detail::from_end_of_t<Rng> to) const
            {
                static_assert(!is_infinite<Rng>::value, "Can't index from the end of an infinite range!");
                EARANGES_EXPECT(0 <= from);
                EARANGES_ASSERT(from <= distance(rng) + to.dist_);
                return slice_base_fn::impl_(static_cast<Rng &&>(rng),
                                            from,
                                            distance(rng) + to.dist_ - from,
                                            range_tag_of<Rng>{});
            }
            // slice(rng, end-4, end-2)
            template(typename Rng)(requires viewable_range<Rng> AND (forward_range<Rng> || (input_range<Rng> && sized_range<Rng>)))
            auto operator()(Rng && rng, detail::from_end_of_t<Rng> from, detail::from_end_of_t<Rng> to) const
            {
                static_assert(!is_infinite<Rng>::value, "Can't index from the end of an infinite range!");
                EARANGES_EXPECT(from.dist_ <= to.dist_);
                return slice_base_fn::impl_(static_cast<Rng &&>(rng),
                                            from.dist_,
                                            to.dist_ - from.dist_,
                                            range_tag_of<Rng>{},
                                            common_range_tag_of<Rng>{});
            }
            // slice(rng, 4, end)
            template(typename Rng)(requires viewable_range<Rng> AND input_range<Rng>)
            auto operator()(Rng && rng, range_difference_t<Rng> from, end_fn) const
            {
                EARANGES_EXPECT(0 <= from);
                return ranges::views::drop_exactly(static_cast<Rng &&>(rng), from);
            }
            // slice(rng, end-4, end)
            template(typename Rng)(requires viewable_range<Rng> AND (forward_range<Rng> || (input_range<Rng> && sized_range<Rng>)))
            auto operator()(Rng && rng, detail::from_end_of_t<Rng> from, end_fn) const
            {
                static_assert(!is_infinite<Rng>::value, "Can't index from the end of an infinite range!");
                return slice_base_fn::impl_(static_cast<Rng &&>(rng),
                                            from.dist_,
                                            -from.dist_,
                                            range_tag_of<Rng>{},
                                            common_range_tag_of<Rng>{});
            }
        };

        struct slice_fn : slice_base_fn
        {
            using slice_base_fn::operator();

            // Overloads for the pipe syntax: rng | views::slice(from,to)
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int from, Int to) const
            {
                return make_view_closure(bind_back(slice_base_fn{}, from, to));
            }
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int from, detail::from_end_<Int> to) const
            {
                return make_view_closure(bind_back(slice_base_fn{}, from, to));
            }
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(detail::from_end_<Int> from, detail::from_end_<Int> to) const
            {
                return make_view_closure(bind_back(slice_base_fn{}, from, to));
            }
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int from, end_fn) const
            {
                return make_view_closure(bind_back(ranges::views::drop_exactly_base_fn{}, from));
            }
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(detail::from_end_<Int> from, end_fn to) const
            {
                return make_view_closure(bind_back(slice_base_fn{}, from, to));
            }
        };

        /// \relates _slice_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(slice_fn, slice)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::slice_view)

#endif