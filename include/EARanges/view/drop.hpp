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

#ifndef EARANGES_VIEW_DROP_HPP
#define EARANGES_VIEW_DROP_HPP

#include <EASTL/type_traits.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/min.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/box.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/interface.hpp>
#include <EARanges/view/subrange.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct EARANGES_EMPTY_BASES drop_view
          : view_interface<drop_view<Rng>,
                           is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private detail::non_propagating_cache<iterator_t<Rng>, drop_view<Rng>,
                                                  !random_access_range<Rng>>
        {
        private:
            using difference_type_ = range_difference_t<Rng>;
            Rng rng_;
            difference_type_ n_;

            template(bool Const = true)(
                requires Const AND range<meta::const_if_c<Const, Rng>>)
                iterator_t<meta::const_if_c<Const, Rng>> //
                get_begin_(eastl::true_type, eastl::true_type) const
            {
                CPP_assert(random_access_range<meta::const_if_c<Const, Rng>>);
                return next(ranges::begin(rng_), n_, ranges::end(rng_));
            }
            iterator_t<Rng> get_begin_(eastl::true_type, eastl::false_type)
            {
                CPP_assert(random_access_range<Rng>);
                return next(ranges::begin(rng_), n_, ranges::end(rng_));
            }
            iterator_t<Rng> get_begin_(eastl::false_type, detail::ignore_t)
            {
                CPP_assert(!random_access_range<Rng>);
                using cache_t =
                    detail::non_propagating_cache<iterator_t<Rng>, drop_view<Rng>>;
                auto & begin_ = static_cast<cache_t &>(*this);
                if(!begin_)
                    begin_ = next(ranges::begin(rng_), n_, ranges::end(rng_));
                return *begin_;
            }

        public:
            drop_view() = default;
            drop_view(Rng rng, difference_type_ n)
              : rng_(eastl::move(rng))
              , n_(n)
            {
                EARANGES_EXPECT(n >= 0);
            }
            iterator_t<Rng> begin()
            {
                return this->get_begin_(meta::bool_<random_access_range<Rng>>{},
                                        eastl::false_type{});
            }
            sentinel_t<Rng> end()
            {
                return ranges::end(rng_);
            }
            template(bool Const = true)(
                requires Const AND random_access_range<meta::const_if_c<Const, Rng>>)
                iterator_t<meta::const_if_c<Const, Rng>> begin() const
            {
                return this->get_begin_(eastl::true_type{}, eastl::true_type{});
            }
            template(bool Const = true)(
                requires Const AND random_access_range<meta::const_if_c<Const, Rng>>)
                sentinel_t<meta::const_if_c<Const, Rng>> end() const
            {
                return ranges::end(rng_);
            }
            CPP_auto_member auto CPP_fun(size)()(const //
                                                 requires sized_range<Rng const>)
            {
                auto const s = ranges::size(rng_);
                auto const n = static_cast<range_size_t<Rng const>>(n_);
                return s < n ? 0 : s - n;
            }
            CPP_auto_member auto CPP_fun(size)()(requires sized_range<Rng>)
            {
                auto const s = ranges::size(rng_);
                auto const n = static_cast<range_size_t<Rng>>(n_);
                return s < n ? 0 : s - n;
            }
            Rng base() const
            {
                return rng_;
            }
        };

        template<typename Rng>
        EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<drop_view<Rng>> = //
            enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template<typename Rng>
        drop_view(Rng &&, range_difference_t<Rng>) -> drop_view<views::all_t<Rng>>;
#endif

        namespace views
        {
            struct drop_base_fn
            {
            private:
                template<typename Rng>
                static auto impl_(Rng && rng, range_difference_t<Rng> n, input_range_tag)
                    -> drop_view<all_t<Rng>>
                {
                    return {all(static_cast<Rng &&>(rng)), n};
                }
                template(typename Rng)(
                    requires borrowed_range<Rng> AND sized_range<
                        Rng>) static subrange<iterator_t<Rng>, sentinel_t<Rng>> //
                    impl_(Rng && rng, range_difference_t<Rng> n, random_access_range_tag)
                {
                    return {begin(rng) + ranges::min(n, distance(rng)), end(rng)};
                }

            public:
                template(typename Rng)(
                    requires viewable_range<Rng> AND input_range<Rng>) auto
                operator()(Rng && rng, range_difference_t<Rng> n) const
                {
                    return drop_base_fn::impl_(
                        static_cast<Rng &&>(rng), n, range_tag_of<Rng>{});
                }
            };

            struct drop_fn : drop_base_fn
            {
                using drop_base_fn::operator();

                template(typename Int)(requires detail::integer_like_<Int>) constexpr auto
                operator()(Int n) const
                {
                    return make_view_closure(bind_back(drop_base_fn{}, n));
                }
            };

            /// \relates drop_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(drop_fn, drop)
        } // namespace views

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::drop_view)

#endif