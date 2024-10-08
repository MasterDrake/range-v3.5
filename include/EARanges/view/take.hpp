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

#ifndef EARANGES_VIEW_TAKE_HPP
#define EARANGES_VIEW_TAKE_HPP

#include <EASTL/type_traits.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/min.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/counted_iterator.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    template<typename Rng>
    struct take_view : view_interface<take_view<Rng>, finite>
    {
    private:
        CPP_assert(view_<Rng>);
        Rng base_ = Rng();
        range_difference_t<Rng> count_ = 0;
        template<bool Const>
        struct sentinel
        {
        private:
            using Base = meta::conditional_t<Const, Rng const, Rng>;
            using CI = counted_iterator<iterator_t<Base>>;
            sentinel_t<Base> end_ = sentinel_t<Base>();

        public:
            sentinel() = default;
            constexpr explicit sentinel(sentinel_t<Base> last)
              : end_(eastl::move(last))
            {}
            template(bool Other)(
                requires Const AND CPP_NOT(Other) AND
                convertible_to<sentinel_t<Rng>,
                               sentinel_t<Base>>)
                constexpr sentinel(sentinel<Other> that)
              : end_(eastl::move(that.end_))
            {}
            constexpr sentinel_t<Base> base() const
            {
                return end_;
            }
#ifdef EARANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // EARANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(sentinel const & x, CI const & y)
            {
                return y.count() == 0 || y.base() == x.end_;
            }
#ifdef EARANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // EARANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator==(CI const & y, sentinel const & x)
            {
                return y.count() == 0 || y.base() == x.end_;
            }
#ifdef EARANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // EARANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(sentinel const & x, CI const & y)
            {
                return y.count() != 0 && y.base() != x.end_;
            }
#ifdef EARANGES_WORKAROUND_MSVC_756601
            template<typename = void>
#endif // EARANGES_WORKAROUND_MSVC_756601
            friend constexpr bool operator!=(CI const & y, sentinel const & x)
            {
                return y.count() != 0 && y.base() != x.end_;
            }
        };

#if EARANGES_CXX_IF_CONSTEXPR < EARANGES_CXX_IF_CONSTEXPR_17
        template<typename Take>
        static auto begin_random_access_(Take & take, eastl::true_type)
        {
            return ranges::begin(take.base_);
        }
        template<typename Take>
        static auto begin_random_access_(Take & take, eastl::false_type)
        {
            auto s = static_cast<range_difference_t<Rng>>(take.size());
            return make_counted_iterator(ranges::begin(take.base_), s);
        }
        template<typename Take>
        static auto begin_sized_(Take & take, eastl::true_type)
        {
            return begin_random_access_(
                take, meta::bool_<random_access_range<decltype((take.base_))>>{});
        }
        template<typename Take>
        static auto begin_sized_(Take & take, eastl::false_type)
        {
            return make_counted_iterator(ranges::begin(take.base_), take.count_);
        }

        template<typename Take>
        static auto end_random_access_(Take & take, eastl::true_type)
        {
            return ranges::begin(take.base_) +
                   static_cast<range_difference_t<Rng>>(take.size());
        }
        static auto end_random_access_(detail::ignore_t, eastl::false_type)
        {
            return default_sentinel;
        }
        template<typename Take>
        static auto end_sized_(Take & take, eastl::true_type, eastl::false_type) // sized
        {
            return end_random_access_(
                take, meta::bool_<random_access_range<decltype((take.base_))>>{});
        }
        static auto end_sized_(detail::ignore_t, eastl::false_type,
                               eastl::true_type) // infinite
        {
            return default_sentinel;
        }
        static auto end_sized_(take_view & take, eastl::false_type, eastl::false_type)
        {
            return sentinel<false>{ranges::end(take.base_)};
        }
        static auto end_sized_(take_view const & take, eastl::false_type, eastl::false_type)
        {
            return sentinel<true>{ranges::end(take.base_)};
        }
#endif
    public:
        take_view() = default;

        constexpr take_view(Rng base, range_difference_t<Rng> cnt)
          : base_(eastl::move(base))
          , count_(cnt)
        {}

        constexpr Rng base() const
        {
            return base_;
        }

        CPP_auto_member
        constexpr auto CPP_fun(begin)()(
            requires(!simple_view<Rng>()))
        {
#if EARANGES_CXX_IF_CONSTEXPR >= EARANGES_CXX_IF_CONSTEXPR_17
            if constexpr(sized_range<Rng>)
                if constexpr(random_access_range<Rng>)
                    return ranges::begin(base_);
                else
                {
                    // cannot always delegate to size() member on GCC with ConceptsTS
#if defined(__cpp_concepts) && __cpp_concepts <= 201507
                    auto s = ranges::min(
                        static_cast<range_difference_t<Rng>>(count_),
                        static_cast<range_difference_t<Rng>>(ranges::size(base_)));
#else
                    auto s = static_cast<range_difference_t<Rng>>(size());
#endif
                    return make_counted_iterator(ranges::begin(base_), s);
                }
            else
                return make_counted_iterator(ranges::begin(base_), count_);
#else
            return begin_sized_(*this, meta::bool_<sized_range<Rng>>{});
#endif
        }

        CPP_auto_member
        constexpr auto CPP_fun(begin)()(const //
            requires range<Rng const>)
        {
#if EARANGES_CXX_IF_CONSTEXPR >= EARANGES_CXX_IF_CONSTEXPR_17
            if constexpr(sized_range<Rng const>)
                if constexpr(random_access_range<Rng const>)
                    return ranges::begin(base_);
                else
                {
                    auto s = static_cast<range_difference_t<Rng>>(size());
                    return make_counted_iterator(ranges::begin(base_), s);
                }
            else
                return make_counted_iterator(ranges::begin(base_), count_);
#else
            return begin_sized_(*this, meta::bool_<sized_range<Rng const>>{});
#endif
        }

        CPP_auto_member
        constexpr auto CPP_fun(end)()(
            requires(!simple_view<Rng>()))
        {
#if EARANGES_CXX_IF_CONSTEXPR >= EARANGES_CXX_IF_CONSTEXPR_17
            if constexpr(sized_range<Rng>)
                if constexpr(random_access_range<Rng>)
                    return ranges::begin(base_) +
                           static_cast<range_difference_t<Rng>>(size());
                else
                    return default_sentinel;
            // Not to spec: Infinite ranges:
            else if constexpr(is_infinite<Rng>::value)
                return default_sentinel;
            else
                return sentinel<false>{ranges::end(base_)};
#else
            return end_sized_(*this, meta::bool_<sized_range<Rng>>{}, is_infinite<Rng>{});
#endif
        }

        CPP_auto_member
        constexpr auto CPP_fun(end)()(const //
            requires range<Rng const>)
        {
#if EARANGES_CXX_IF_CONSTEXPR >= EARANGES_CXX_IF_CONSTEXPR_17
            if constexpr(sized_range<Rng const>)
                if constexpr(random_access_range<Rng const>)
                    return ranges::begin(base_) +
                           static_cast<range_difference_t<Rng>>(size());
                else
                    return default_sentinel;
            // Not to spec: Infinite ranges:
            else if constexpr(is_infinite<Rng const>::value)
                return default_sentinel;
            else
                return sentinel<true>{ranges::end(base_)};
#else
            return end_sized_(
                *this, meta::bool_<sized_range<Rng const>>{}, is_infinite<Rng const>{});
#endif
        }

        CPP_auto_member
        constexpr auto CPP_fun(size)()(
            requires sized_range<Rng>)
        {
            auto n = ranges::size(base_);
            return ranges::min(n, static_cast<decltype(n)>(count_));
        }
        CPP_auto_member
        constexpr auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            auto n = ranges::size(base_);
            return ranges::min(n, static_cast<decltype(n)>(count_));
        }
    };

    template<typename Rng>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<take_view<Rng>> = //
        enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    take_view(Rng &&, range_difference_t<Rng>) -> take_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct take_base_fn
        {
            template(typename Rng)(requires viewable_range<Rng>)
            take_view<all_t<Rng>> operator()(Rng && rng, range_difference_t<Rng> n) const
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
        };

        struct take_fn : take_base_fn
        {
            using take_base_fn::operator();

            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int n) const
            {
                return make_view_closure(bind_back(take_base_fn{}, n));
            }
        };

        /// \relates take_fn
        EARANGES_INLINE_VARIABLE(take_fn, take)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::take_view)

#endif
