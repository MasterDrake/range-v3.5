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

#ifndef EARANGES_VIEW_REPLACE_IF_HPP
#define EARANGES_VIEW_REPLACE_IF_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/utility/compressed_pair.hpp>//TODO: ERRORE?
//#include "../../../../../../../../Common_3/Utilities/ThirdParty/OpenSource/EASTL/bonus/compressed_pair.h"
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Pred, typename Val>
        struct replacer_if_fn : eastl::compressed_pair<semiregular_box_t<Pred>, Val>
        {
        private:
            using base_t = eastl::compressed_pair<semiregular_box_t<Pred>, Val>;
            using base_t::first;
            using base_t::second;

        public:
            replacer_if_fn() = default;
            constexpr replacer_if_fn(Pred pred, Val new_value)
              : base_t{eastl::move(pred), eastl::move(new_value)}
            {}

            template<typename I>
            [[noreturn]] common_type_t<decay_t<unwrap_reference_t<Val const &>>,
                                       iter_value_t<I>> &
            operator()(copy_tag, I const &) const
            {
                EARANGES_EXPECT(false);
            }

            template(typename I)(
                requires (!invocable<Pred const &, iter_reference_t<I>>))
            common_reference_t<unwrap_reference_t<Val const &>, iter_reference_t<I>> //
            operator()(I const & i)
            {
                auto && x = *i;
                if(invoke(first(), (decltype(x) &&)x)) //
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
            template(typename I)(
                requires invocable<Pred const &, iter_reference_t<I>>)
            common_reference_t<unwrap_reference_t<Val const &>, iter_reference_t<I>> //
            operator()(I const & i) const
            {
                auto && x = *i;
                if(invoke(first(), (decltype(x) &&)x)) //
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }

            template(typename I)(
                requires (!invocable<Pred const &, iter_rvalue_reference_t<I>>))
            common_reference_t<
                unwrap_reference_t<Val const &>, //
                iter_rvalue_reference_t<I>> //
            operator()(move_tag, I const & i)
            {
                auto && x = iter_move(i);
                if(invoke(first(), (decltype(x) &&)x)) //
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
            template(typename I)(
                requires invocable<Pred const &, iter_rvalue_reference_t<I>>)
            common_reference_t< //
                unwrap_reference_t<Val const &>, //
                iter_rvalue_reference_t<I>> //
            operator()(move_tag, I const & i) const
            {
                auto && x = iter_move(i);
                if(invoke(first(), (decltype(x) &&)x)) //
                    return unwrap_reference(second());
                return (decltype(x) &&)x;
            }
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct replace_if_base_fn
        {
            template(typename Rng, typename Pred, typename Val)(
                requires viewable_range<Rng> AND input_range<Rng> AND
                    indirect_unary_predicate<Pred, iterator_t<Rng>> AND
                    common_with<detail::decay_t<unwrap_reference_t<Val const &>>,
                                range_value_t<Rng>> AND
                    common_reference_with<unwrap_reference_t<Val const &>,
                                          range_reference_t<Rng>> AND
                    common_reference_with<unwrap_reference_t<Val const &>,
                                          range_rvalue_reference_t<Rng>>)
            constexpr replace_if_view<all_t<Rng>, Pred, Val> //
            operator()(Rng && rng, Pred pred, Val new_value) const
            {
                return {all(static_cast<Rng &&>(rng)), {eastl::move(pred), eastl::move(new_value)}};
            }
        };

        struct replace_if_fn : replace_if_base_fn
        {
            using replace_if_base_fn::operator();

            template<typename Pred, typename Val>
            constexpr auto operator()(Pred pred, Val new_value) const
            {
                return make_view_closure(bind_back(replace_if_base_fn{}, eastl::move(pred), eastl::move(new_value)));
            }
        };

        /// \relates replace_if_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(replace_if_fn, replace_if)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif