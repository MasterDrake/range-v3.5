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

#ifndef EARANGES_VIEW_REVERSE_HPP
#define EARANGES_VIEW_REVERSE_HPP

#include <EASTL/iterator.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/reverse_iterator.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/box.hpp>
#include <EARanges/utility/get.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adaptor.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct EARANGES_EMPTY_BASES reverse_view
      : view_interface<reverse_view<Rng>, range_cardinality<Rng>::value>
      , private detail::non_propagating_cache<iterator_t<Rng>, reverse_view<Rng>,
                                              !common_range<Rng>>
    {
    private:
        CPP_assert(bidirectional_range<Rng>);
        Rng rng_;
        constexpr reverse_iterator<iterator_t<Rng>> begin_(eastl::true_type)
        {
            return make_reverse_iterator(ranges::end(rng_));
        }
        constexpr reverse_iterator<iterator_t<Rng>> begin_(eastl::false_type)
        {
            using cache_t =
                detail::non_propagating_cache<iterator_t<Rng>, reverse_view<Rng>>;
            auto & end_ = static_cast<cache_t &>(*this);
            if(!end_)
            {
#if defined(_MSC_VER)
                auto tmp = ranges::begin(rng_);
                auto e = ranges::end(rng_);
                while(tmp != e)
                    ++tmp;
#else
                auto tmp = ranges::next(ranges::begin(rng_), ranges::end(rng_));
#endif
                end_ = eastl::move(tmp);
            }
            return make_reverse_iterator(*end_);
        }

    public:
        reverse_view() = default;
        constexpr explicit reverse_view(Rng rng)
          : rng_(detail::move(rng))
        {}
        Rng base() const
        {
            return rng_;
        }
        constexpr reverse_iterator<iterator_t<Rng>> begin()
        {
            return begin_(meta::bool_<(bool)common_range<Rng>>{});
        }
        template(bool Const = true)(
            requires Const AND common_range<meta::const_if_c<Const, Rng>>)
        constexpr reverse_iterator<iterator_t<meta::const_if_c<Const, Rng>>> begin() const
        {
            return make_reverse_iterator(ranges::end(rng_));
        }
        constexpr reverse_iterator<iterator_t<Rng>> end()
        {
            return make_reverse_iterator(ranges::begin(rng_));
        }
        template(bool Const = true)(
            requires Const AND common_range<meta::const_if_c<Const, Rng>>)
        constexpr reverse_iterator<iterator_t<meta::const_if_c<Const, Rng>>> end() const
        {
            return make_reverse_iterator(ranges::begin(rng_));
        }
        CPP_auto_member
        constexpr auto CPP_fun(size)()(
            requires sized_range<Rng>)
        {
            return ranges::size(rng_);
        }
        CPP_auto_member
        constexpr auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            return ranges::size(rng_);
        }
    };

    template<typename Rng>
    struct reverse_view<reverse_view<Rng>> : Rng
    {
        CPP_assert(bidirectional_range<Rng>);
        CPP_assert(same_as<detail::decay_t<decltype(eastl::declval<reverse_view<Rng>>().base())>, Rng>);

        reverse_view() = default;
        constexpr explicit reverse_view(reverse_view<Rng> rng) : Rng(rng.base())
        {}

        constexpr reverse_view<Rng> base() const
        {
            return reverse_view<Rng>{*this};
        }
    };

    template<typename Rng>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<reverse_view<Rng>> = enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    reverse_view(Rng &&) //
        -> reverse_view<views::all_t<Rng>>;

    template<typename Rng>
    reverse_view(reverse_view<Rng>) -> reverse_view<reverse_view<Rng>>;
#endif

    namespace views
    {
        struct reverse_fn
        {
            template(typename Rng)(requires viewable_range<Rng> AND bidirectional_range<Rng>)
            constexpr reverse_view<all_t<Rng>> operator()(Rng && rng) const
            {
                return reverse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        /// \relates reverse_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(view_closure<reverse_fn>, reverse)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::reverse_view)

#endif