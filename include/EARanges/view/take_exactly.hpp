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

#ifndef EARANGES_VIEW_TAKE_EXACTLY_HPP
#define EARANGES_VIEW_TAKE_EXACTLY_HPP

#include <EASTL/type_traits.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/counted_iterator.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/counted.hpp>
#include <EARanges/view/interface.hpp>
#include <EARanges/view/subrange.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Rng>
        struct is_random_access_common_ : meta::bool_<(bool)random_access_range<Rng> && (bool)common_range<Rng>>
        {};

        // BUGBUG Per the discussion in https://github.com/ericniebler/stl2/issues/63,
        // it's unclear if we can infer anything from random_access_range<Rng> &&
        // common_range<Rng>
        template<typename Rng, bool IsRandomAccessCommon /*= is_random_access_common_<Rng>::value*/>
        struct take_exactly_view_
          : view_interface<take_exactly_view_<Rng, IsRandomAccessCommon>, finite>
        {
        private:
            Rng rng_;
            range_difference_t<Rng> n_;

        public:
            take_exactly_view_() = default;
            take_exactly_view_(Rng rng, range_difference_t<Rng> n)
              : rng_(eastl::move(rng))
              , n_(n)
            {
                EARANGES_EXPECT(n >= 0);
            }
            counted_iterator<iterator_t<Rng>> begin()
            {
                return {ranges::begin(rng_), n_};
            }
            template(typename BaseRng = Rng)(
                requires range<BaseRng const>)
            counted_iterator<iterator_t<BaseRng const>> begin() const
            {
                return {ranges::begin(rng_), n_};
            }
            default_sentinel_t end() const
            {
                return {};
            }
            auto size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(n_);
            }
            Rng base() const
            {
                return rng_;
            }
        };

        template<typename Rng>
        struct take_exactly_view_<Rng, true>
          : view_interface<take_exactly_view_<Rng, true>, finite>
        {
        private:
            Rng rng_;
            range_difference_t<Rng> n_;

        public:
            take_exactly_view_() = default;
            take_exactly_view_(Rng rng, range_difference_t<Rng> n)
              : rng_(eastl::move(rng))
              , n_(n)
            {
                EARANGES_EXPECT(n >= 0);
                EARANGES_EXPECT(!(bool)sized_range<Rng> || n <= ranges::distance(rng_));
            }
            iterator_t<Rng> begin()
            {
                return ranges::begin(rng_);
            }
            iterator_t<Rng> end()
            {
                return ranges::begin(rng_) + n_;
            }
            CPP_auto_member
            auto CPP_fun(begin)()(const //
                requires range<Rng const>)
            {
                return ranges::begin(rng_);
            }
            CPP_auto_member
            auto CPP_fun(end)()(const //
                requires range<Rng const>)
            {
                return ranges::begin(rng_) + n_;
            }
            detail::iter_size_t<iterator_t<Rng>> size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(n_);
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
    using take_exactly_view = detail::take_exactly_view_<Rng>;

    template<typename Rng, bool B>
    EARANGES_INLINE_VAR constexpr bool //
        enable_borrowed_range<detail::take_exactly_view_<Rng, B>> = //
            enable_borrowed_range<Rng>;

    namespace views
    {
        struct take_exactly_base_fn
        {
        private:
            template<typename Rng>
            static constexpr take_exactly_view<all_t<Rng>> impl_(
                Rng && rng, range_difference_t<Rng> n, input_range_tag)
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
            template(typename Rng)(
                requires borrowed_range<Rng>)
            static constexpr subrange<iterator_t<Rng>> impl_(Rng && rng,
                                                             range_difference_t<Rng> n,
                                                             random_access_range_tag)
            {
                return {begin(rng), next(begin(rng), n)};
            }

        public:
            template(typename Rng)(
                requires viewable_range<Rng> AND input_range<Rng>)
            constexpr auto operator()(Rng && rng, range_difference_t<Rng> n) const
            {
                return take_exactly_base_fn::impl_(static_cast<Rng &&>(rng), n, range_tag_of<Rng>{});
            }
        };

        struct take_exactly_fn : take_exactly_base_fn
        {
            using take_exactly_base_fn::operator();

            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int n) const
            {
                return make_view_closure(bind_back(take_exactly_base_fn{}, n));
            }
        };

        /// \relates take_exactly_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(take_exactly_fn, take_exactly)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::detail::take_exactly_view_)

#endif