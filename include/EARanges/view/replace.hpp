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

#ifndef EARANGES_VIEW_REPLACE_HPP
#define EARANGES_VIEW_REPLACE_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
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
        template<typename Val1, typename Val2>
        struct replacer_fn
        {
        private:
            Val1 old_value_;
            Val2 new_value_;

        public:
            replacer_fn() = default;
            constexpr replacer_fn(Val1 old_value, Val2 new_value)
              : old_value_(eastl::move(old_value))
              , new_value_(eastl::move(new_value))
            {}

            template<typename I>
            [[noreturn]] common_type_t<decay_t<unwrap_reference_t<Val2 const &>>, iter_value_t<I>> &
            operator()(copy_tag, I const &) const
            {
                EARANGES_EXPECT(false);
            }

            template<typename I>
            common_reference_t<unwrap_reference_t<Val2 const &>, iter_reference_t<I>>
            operator()(I const & i) const
            {
                auto && x = *i;
                if(x == unwrap_reference(old_value_))
                    return unwrap_reference(new_value_);
                return ((decltype(x) &&)x);
            }

            template<typename I>
            common_reference_t<unwrap_reference_t<Val2 const &>, iter_rvalue_reference_t<I>>
            operator()(move_tag, I const & i) const
            {
                auto && x = iter_move(i);
                if(x == unwrap_reference(old_value_))
                    return unwrap_reference(new_value_);
                return ((decltype(x) &&)x);
            }
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct replace_base_fn
        {
            template(typename Rng, typename Val1, typename Val2)(
                requires viewable_range<Rng> AND input_range<Rng> AND
                    same_as<
                        detail::decay_t<unwrap_reference_t<Val1>>,
                        detail::decay_t<unwrap_reference_t<Val2>>> AND
                    equality_comparable_with<
                        detail::decay_t<unwrap_reference_t<Val1>>,
                        range_value_t<Rng>> AND
                    common_with<detail::decay_t<unwrap_reference_t<Val2 const &>>,
                                range_value_t<Rng>> AND
                    common_reference_with<unwrap_reference_t<Val2 const &>,
                                            range_reference_t<Rng>> AND
                    common_reference_with<
                        unwrap_reference_t<Val2 const &>,
                        range_rvalue_reference_t<Rng>>)
            constexpr replace_view< //
                all_t<Rng>, //
                detail::decay_t<Val1>, //
                detail::decay_t<Val2>> //
            operator()(Rng && rng, Val1 && old_value,
                                      Val2 && new_value) const //
            {
                return {
                    all(static_cast<Rng &&>(rng)),
                    {static_cast<Val1 &&>(old_value), static_cast<Val2 &&>(new_value)}};
            }
        };

        struct replace_fn : replace_base_fn
        {
            using replace_base_fn::operator();

            template(typename Val1, typename Val2)(requires same_as<detail::decay_t<unwrap_reference_t<Val1>>, detail::decay_t<unwrap_reference_t<Val2>>>)
            constexpr auto operator()(Val1 old_value, Val2 new_value) const
            {
                return make_view_closure(bind_back(replace_base_fn{}, eastl::move(old_value), eastl::move(new_value)));
            }
        };

        /// \relates replace_fn
        /// \ingroup group-views
        EARANGES_INLINE_VARIABLE(replace_fn, replace)
    } // namespace views
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif