/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_DROP_LAST_HPP
#define EARANGES_VIEW_DROP_LAST_HPP

#include <EASTL/type_traits.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/counted_iterator.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adaptor.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/interface.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    /// \cond
    namespace detail
    {
        namespace drop_last_view
        {
            template<typename Rng>
            range_size_t<Rng> get_size(Rng & rng, range_difference_t<Rng> n_)
            {
                EARANGES_EXPECT(n_ >= 0);
                range_size_t<Rng> const initial_size = ranges::size(rng);
                range_size_t<Rng> const n = static_cast<range_size_t<Rng>>(n_);
                return initial_size > n ? initial_size - n : 0;
            }

            template(typename Rng)(requires random_access_range<Rng> AND sized_range<Rng>)
            iterator_t<Rng> get_end(Rng & rng, range_difference_t<Rng> n, int)
            {
                return begin(rng) + static_cast<range_difference_t<Rng>>(drop_last_view::get_size(rng, n));
            }
            template(typename Rng)(requires bidirectional_range<Rng> AND common_range<Rng>)
            iterator_t<Rng> get_end(Rng & rng, range_difference_t<Rng> n, long)
            {
                return prev(end(rng), n, begin(rng));
            }

            enum class mode_enum
            {
                bidi,
                forward,
                sized,
                invalid
            };

            template<mode_enum Mode>
            using mode_t = eastl::integral_constant<mode_enum, Mode>;

            using mode_bidi = mode_t<mode_enum::bidi>;
            using mode_forward = mode_t<mode_enum::forward>;
            using mode_sized = mode_t<mode_enum::sized>;
            using mode_invalid = mode_t<mode_enum::invalid>;

            template<typename Rng>
            constexpr mode_enum get_mode() noexcept
            {
                // keep range bound
                // Sized Bidi O(N)
                return (random_access_range<Rng> && view_<Rng> && sized_range<Rng>) ||
                               (bidirectional_range<Rng> && view_<Rng> &&
                                common_range<Rng>)
                           ? mode_enum::bidi                        //
                           : sized_range<Rng> && view_<Rng>         //
                                 ? mode_enum::sized                 //
                                 : forward_range<Rng> && view_<Rng> //
                                       ? mode_enum::forward         //
                                       : mode_enum::invalid;        //

                // max performance
                // Sized Bidi O(1)
                // Sized Bidi use mode::sized instead of mode::bidi - thus become unbound.
                /*return (random_access_range<Rng> && view_<Rng> && sized_range<Rng> &&
                   view_<Rng>) || (bidirectional_range<Rng> && view_<Rng> &&
                   common_range<Rng> && view_<Rng>) ? mode::bidi : sized_range<Rng> &&
                   view_<Rng> ? mode::sized : bidirectional_range<Rng> && view_<Rng> &&
                   common_range<Rng> && view_<Rng> ? mode::bidi : forward_range<Rng> &&
                   view_<Rng> ? mode::forward : mode::invalid;*/
            }

            template<typename Rng>
            using mode_of = mode_t<drop_last_view::get_mode<Rng>()>;
        } // namespace drop_last_view
    }     // namespace detail
    /// \endcond

    template<typename Rng, typename = detail::drop_last_view::mode_of<Rng>>
    struct drop_last_view
    {};

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_bidi>
      : view_interface<drop_last_view<Rng, detail::drop_last_view::mode_bidi>,
                       is_finite<Rng>::value
                           ? finite
                           : range_cardinality<Rng>::value> // finite at best
    {
        CPP_assert(
            (random_access_range<Rng> && view_<Rng> && sized_range<Rng>) ||
            (bidirectional_range<Rng> && view_<Rng> && common_range<Rng>));

    private:
        friend range_access;
        using difference_t = range_difference_t<Rng>;

        Rng rng_;
        difference_t n_;
        detail::non_propagating_cache<iterator_t<Rng>> end_;

    public:
        drop_last_view() = default;
        constexpr drop_last_view(Rng rng, difference_t n)
          : rng_(eastl::move(rng))
          , n_(n)
        {
            EARANGES_EXPECT(n >= 0);
        }

        iterator_t<Rng> begin()
        {
            return ranges::begin(rng_);
        }
        sentinel_t<Rng> end()
        {
            if(!end_)
                end_ = detail::drop_last_view::get_end(rng_, n_, 0);
            return *end_;
        }
        template(typename CRng = Rng const)(
            requires random_access_range<CRng> AND sized_range<CRng>)
        iterator_t<CRng> begin() const
        {
            return ranges::begin(rng_);
        }
        template(typename CRng = Rng const)(
            requires random_access_range<CRng> AND sized_range<CRng>)
        iterator_t<CRng> end() const
        {
            return detail::drop_last_view::get_end(rng_, n_, 0);
        }

        CPP_auto_member
        auto CPP_fun(size)()(
            requires sized_range<Rng>)
        {
            return detail::drop_last_view::get_size(rng_, n_);
        }
        CPP_auto_member
        auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            return detail::drop_last_view::get_size(rng_, n_);
        }

        Rng & base()
        {
            return rng_;
        }
        Rng const & base() const
        {
            return rng_;
        }
    };

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_forward>
      : view_adaptor<drop_last_view<Rng, detail::drop_last_view::mode_forward>, Rng,
                     is_finite<Rng>::value
                         ? finite
                         : range_cardinality<Rng>::value> // finite at best (but
                                                          // unknown is expected)
    {
        CPP_assert(forward_range<Rng> && view_<Rng>);

    private:
        friend range_access;

        using difference_t = range_difference_t<Rng>;
        difference_t n_;
        detail::non_propagating_cache<iterator_t<Rng>> probe_begin;

        struct adaptor : adaptor_base
        {
            iterator_t<Rng> probe_;

            adaptor() = default;
            adaptor(iterator_t<Rng> probe_first)
              : probe_(eastl::move(probe_first))
            {}
            void next(iterator_t<Rng> & it)
            {
                ++it;
                ++probe_;
            }
        };

        struct sentinel_adaptor : adaptor_base
        {
            template<typename I, typename S>
            bool empty(I const &, adaptor const & ia, S const & s) const
            {
                return ia.probe_ == s;
            }
        };

        adaptor begin_adaptor()
        {
            if(!probe_begin)
                probe_begin = next(begin(this->base()), n_, end(this->base()));
            return {*probe_begin};
        }
        sentinel_adaptor end_adaptor()
        {
            return {};
        }

    public:
        drop_last_view() = default;
        constexpr drop_last_view(Rng rng, difference_t n)
          : drop_last_view::view_adaptor(eastl::move(rng))
          , n_(n)
        {
            EARANGES_EXPECT(n >= 0);
        }

        CPP_auto_member
        auto CPP_fun(size)()(
            requires sized_range<Rng>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
        CPP_auto_member
        auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
    };

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_sized>
      : view_interface<drop_last_view<Rng, detail::drop_last_view::mode_sized>, finite>
    {
        CPP_assert(sized_range<Rng> && view_<Rng>);

    private:
        friend range_access;

        using difference_t = range_difference_t<Rng>;
        Rng rng_;
        difference_t n_;

    public:
        drop_last_view() = default;
        constexpr drop_last_view(Rng rng, difference_t n)
          : rng_(eastl::move(rng))
          , n_(n)
        {
            EARANGES_EXPECT(n >= 0);
        }

        counted_iterator<iterator_t<Rng>> begin()
        {
            return {ranges::begin(rng_), static_cast<difference_t>(size())};
        }
        template(typename CRng = Rng const)(
            requires sized_range<CRng>)
        counted_iterator<iterator_t<CRng>> begin() const
        {
            return {ranges::begin(rng_), static_cast<difference_t>(size())};
        }
        default_sentinel_t end() const
        {
            return {};
        }
        range_size_t<Rng> size()
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
        CPP_auto_member
        auto CPP_fun(size)()(const //
            requires sized_range<Rng const>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }

        Rng & base()
        {
            return rng_;
        }
        Rng const & base() const
        {
            return rng_;
        }
    };

    template<typename Rng, typename T>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<drop_last_view<Rng, T>> = //
        enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    drop_last_view(Rng &&, range_difference_t<Rng>) -> drop_last_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct drop_last_base_fn
        {
            template(typename Rng)(requires sized_range<Rng> || forward_range<Rng>)
            constexpr auto operator()(Rng && rng, range_difference_t<Rng> n) const -> drop_last_view<all_t<Rng>>
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
        };

        struct drop_last_fn : drop_last_base_fn
        {
            using drop_last_base_fn::operator();

            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int n) const
            {
                return make_view_closure(bind_back(drop_last_base_fn{}, n));
            }
        };

        EARANGES_INLINE_VARIABLE(drop_last_fn, drop_last)
    } // namespace views

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>
#include "../detail/satisfy_boost_range.hpp"
EARANGES_SATISFY_BOOST_RANGE(::ranges::drop_last_view)

#endif