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

#ifndef EARANGES_VIEW_REMOVE_IF_HPP
#define EARANGES_VIEW_REMOVE_IF_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/compose.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/box.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adaptor.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct EARANGES_EMPTY_BASES remove_if_view
          : view_adaptor<remove_if_view<Rng, Pred>, Rng,
                         is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private box<semiregular_box_t<Pred>>
        {
            remove_if_view() = default;
            constexpr remove_if_view(Rng rng, Pred pred)
              : remove_if_view::view_adaptor{detail::move(rng)}
              , remove_if_view::box(detail::move(pred))
            {}

        private:
            friend range_access;

            struct adaptor : adaptor_base
            {
                adaptor() = default;
                constexpr adaptor(remove_if_view * rng) noexcept
                  : rng_(rng)
                {}
                static constexpr iterator_t<Rng> begin(remove_if_view & rng)
                {
                    return *rng.begin_;
                }
                constexpr void next(iterator_t<Rng> & it) const
                {
                    EARANGES_ASSERT(it != ranges::end(rng_->base()));
                    rng_->satisfy_forward(++it);
                }
                CPP_member
                constexpr auto prev(iterator_t<Rng> & it) const //
                    -> CPP_ret(void)(requires bidirectional_range<Rng>)
                {
                    rng_->satisfy_reverse(it);
                }
                void advance() = delete;
                void distance_to() = delete;

            private:
                remove_if_view * rng_;
            };
            constexpr adaptor begin_adaptor()
            {
                cache_begin();
                return {this};
            }
            CPP_member
            constexpr auto end_adaptor() const noexcept //
                -> CPP_ret(adaptor_base)(requires(!common_range<Rng>))
            {
                return {};
            }
            CPP_member
            constexpr auto end_adaptor() //
                -> CPP_ret(adaptor)(requires common_range<Rng>)
            {
                if(bidirectional_range<Rng>)
                    cache_begin();
                return {this};
            }

            constexpr void satisfy_forward(iterator_t<Rng> & it)
            {
                auto const last = ranges::end(this->base());
                auto & pred = this->remove_if_view::box::get();
                while(it != last && invoke(pred, *it))
                    ++it;
            }
            constexpr void satisfy_reverse(iterator_t<Rng> & it)
            {
                EARANGES_ASSERT(begin_);
                auto const & first = *begin_;
                auto & pred = this->remove_if_view::box::get();
                do
                {
                    EARANGES_ASSERT(it != first);
                    (void)first;
                    --it;
                } while(invoke(pred, *it));
            }

            constexpr void cache_begin()
            {
                if(begin_)
                    return;
                auto it = ranges::begin(this->base());
                satisfy_forward(it);
                begin_.emplace(eastl::move(it));
            }

            detail::non_propagating_cache<iterator_t<Rng>> begin_;
        };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename Rng, typename Pred)(requires copy_constructible<Pred>)
            remove_if_view(Rng &&, Pred) -> remove_if_view<views::all_t<Rng>, Pred>;
#endif

        namespace views
        {
            /// Given a source range, unary predicate, and optional projection,
            /// present a view of the elements that do not satisfy the predicate.
            struct remove_if_base_fn
            {
                template(typename Rng, typename Pred)(
                    requires viewable_range<Rng> AND input_range<Rng> AND
                        indirect_unary_predicate<
                            Pred, iterator_t<Rng>>) constexpr remove_if_view<all_t<Rng>,
                                                                             Pred>
                operator()(Rng && rng, Pred pred) const
                {
                    return remove_if_view<all_t<Rng>, Pred>{all(static_cast<Rng &&>(rng)),
                                                            eastl::move(pred)};
                }
                template(typename Rng, typename Pred, typename Proj)(
                    requires viewable_range<Rng> AND input_range<Rng> AND
                        indirect_unary_predicate<
                            Pred,
                            projected<iterator_t<Rng>,
                                      Proj>>) constexpr remove_if_view<all_t<Rng>,
                                                                       composed<Pred,
                                                                                Proj>> //
                operator()(Rng && rng, Pred pred, Proj proj) const
                {
                    return remove_if_view<all_t<Rng>, composed<Pred, Proj>>{
                        all(static_cast<Rng &&>(rng)),
                        compose(eastl::move(pred), eastl::move(proj))};
                }
            };

            struct remove_if_bind_fn
            {
                template<typename Pred>
                constexpr auto operator()(
                    Pred pred) const // TODO: underconstrained - Eric Niebler
                {
                    return make_view_closure(
                        bind_back(remove_if_base_fn{}, eastl::move(pred)));
                }
                template(typename Pred, typename Proj)(
                    requires(!range<Pred>)) // TODO: underconstrained - Eric Niebler
                    constexpr auto
                    operator()(Pred && pred, Proj proj) const
                {
                    return make_view_closure(bind_back(remove_if_base_fn{},
                                                       static_cast<Pred &&>(pred),
                                                       eastl::move(proj)));
                }
            };

            struct EARANGES_EMPTY_BASES remove_if_fn
              : remove_if_base_fn
              , remove_if_bind_fn
            {
                using remove_if_base_fn::operator();
                using remove_if_bind_fn::operator();
            };

            /// \relates remove_if_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(remove_if_fn, remove_if)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::remove_if_view)

#include <EARanges/detail/epilogue.hpp>

#endif
