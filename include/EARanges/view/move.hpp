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

#ifndef EARANGES_VIEW_MOVE_HPP
#define EARANGES_VIEW_MOVE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adaptor.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct move_view : view_adaptor<move_view<Rng>, Rng>
        {
        private:
            friend range_access;
            template<bool Const>
            struct adaptor : adaptor_base
            {
                adaptor() = default;
                template(bool Other)(requires Const AND CPP_NOT(Other)) //
                    constexpr adaptor(adaptor<Other>)
                {}
                using CRng = meta::const_if_c<Const, Rng>;
                using value_type = range_value_t<Rng>;
                range_rvalue_reference_t<CRng> read(iterator_t<CRng> const & it) const
                {
                    return ranges::iter_move(it);
                }
                range_rvalue_reference_t<CRng> iter_move(
                    iterator_t<CRng> const & it) const
                {
                    return ranges::iter_move(it);
                }
            };
            adaptor<simple_view<Rng>()> begin_adaptor()
            {
                return {};
            }
            adaptor<simple_view<Rng>()> end_adaptor()
            {
                return {};
            }
            CPP_member
            auto begin_adaptor() const //
                -> CPP_ret(adaptor<true>)(requires input_range<Rng const>)
            {
                return {};
            }
            CPP_member
            auto end_adaptor() const //
                -> CPP_ret(adaptor<true>)(requires input_range<Rng const>)
            {
                return {};
            }

        public:
            move_view() = default;
            explicit move_view(Rng rng)
              : move_view::view_adaptor{eastl::move(rng)}
            {}
            CPP_auto_member auto CPP_fun(size)()(const //
                                                 requires sized_range<Rng const>)
            {
                return ranges::size(this->base());
            }
            CPP_auto_member auto CPP_fun(size)()(requires sized_range<Rng>)
            {
                return ranges::size(this->base());
            }
        };

        template<typename Rng>
        EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<move_view<Rng>> =
            enable_borrowed_range<Rng>;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template<typename Rng>
        move_view(Rng &&) //
            ->move_view<views::all_t<Rng>>;
#endif

        namespace views
        {
            struct move_fn
            {
                template(typename Rng)(requires viewable_range<Rng> AND
                                           input_range<Rng>) move_view<all_t<Rng>>
                operator()(Rng && rng) const
                {
                    return move_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }
            };

            /// \relates move_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(view_closure<move_fn>, move)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::move_view)

#endif
