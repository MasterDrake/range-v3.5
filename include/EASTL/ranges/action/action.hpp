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

#ifndef RANGES_V3_CONTAINER_ACTION_HPP
#define RANGES_V3_CONTAINER_ACTION_HPP

//#include <EASTL/type_traits.h>
#include "EASTL/type_traits.h"

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"
     
#include "../action/concepts.hpp"
#include "../functional/compose.hpp"
#include "../functional/concepts.hpp"
#include "../functional/invoke.hpp"
#include "../functional/reference_wrapper.hpp"
#include "../functional/pipeable.hpp"
#include "../range/concepts.hpp"
#include "../utility/move.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    struct make_action_closure_fn
    {
        template<typename Fun>
        constexpr actions::action_closure<Fun> operator()(Fun fun) const
        {
            return actions::action_closure<Fun>{static_cast<Fun &&>(fun)};
        }
    };

    /// \sa make_action_closure_fn
    RANGES_INLINE_VARIABLE(make_action_closure_fn, make_action_closure)

    /// \cond
    namespace detail
    {
        struct action_closure_base_
        {};
    }
    /// \endcond

    /// \concept invocable_action_closure_
    /// \brief The \c invocable_action_closure_ concept
    template(typename ActionFn, typename Rng)(
    concept (invocable_action_closure_)(ActionFn, Rng),
        !derived_from<invoke_result_t<ActionFn, Rng>, detail::action_closure_base_>
    );
    /// \concept invocable_action_closure
    /// \brief The \c invocable_action_closure concept
    template<typename ActionFn, typename Rng>
    CPP_concept invocable_action_closure =
        invocable<ActionFn, Rng> &&
        CPP_concept_ref(ranges::invocable_action_closure_, ActionFn, Rng);

    namespace actions
    {
        struct RANGES_STRUCT_WITH_ADL_BARRIER(action_closure_base): detail::action_closure_base_
        {
            // clang-format off
            // Piping requires things are passed by value.
            template(typename Rng, typename ActionFn)(requires (!eastl::is_lvalue_reference<Rng>::value) AND range<Rng> AND invocable_action_closure<ActionFn, Rng &>)
            friend constexpr auto operator|(Rng && rng, action_closure<ActionFn> act)
            {
                return aux::move(static_cast<ActionFn &&>(act)(rng));
            }

#ifndef RANGES_WORKAROUND_CLANG_43400
            template<typename Rng, typename ActionFn>   // ******************************
            friend constexpr auto                       // ******************************
            operator|(Rng &,                            // ********* READ THIS **********
                      action_closure<ActionFn> const &) // ****** IF YOUR COMPILE *******
                -> CPP_broken_friend_ret(Rng)(          // ******** BREAKS HERE *********
                    requires range<Rng>) = delete;      // ******************************
            // **************************************************************************
            // *    When piping a range into an action, the range must be moved in.     *
            // **************************************************************************
#endif // RANGES_WORKAROUND_CLANG_43400

            template<typename ActionFn, typename Pipeable>
            friend constexpr auto operator|(action_closure<ActionFn> act, Pipeable pipe) -> CPP_broken_friend_ret(action_closure<composed<Pipeable, ActionFn>>)(requires (is_pipeable_v<Pipeable>))
            {
                return make_action_closure(compose(static_cast<Pipeable &&>(pipe), static_cast<ActionFn &&>(act)));
            }

            template<typename Rng, typename ActionFn>
            friend constexpr auto operator|=(Rng & rng, action_closure<ActionFn> act) //
                -> CPP_broken_friend_ret(Rng &)(requires range<Rng> && invocable<ActionFn, Rng &>)
            {
                static_cast<ActionFn &&>(act)(rng);
                return rng;
            }
            // clang-format on
        };

#ifdef RANGES_WORKAROUND_CLANG_43400
        // clang-format off
        namespace RANGES_ADL_BARRIER_FOR(action_closure_base)
        {
            template(typename Rng, typename ActionFn)(  // *******************************
                requires range<Rng>)                    // *******************************
            constexpr Rng                               // ********** READ THIS **********
            operator|(Rng &,                            // ******* IF YOUR COMPILE *******
                      action_closure<ActionFn> const &) // ********* BREAKS HERE *********
                = delete;                               // *******************************
            // ***************************************************************************
            // *    When piping a range into an action, the range must be moved in.      *
            // ***************************************************************************
        } // namespace RANGES_ADL_BARRIER_FOR(action_closure_base)
        // clang-format on
#endif    // RANGES_WORKAROUND_CLANG_43400

        template<typename ActionFn>
        struct RANGES_EMPTY_BASES action_closure: action_closure_base, ActionFn
        {
            action_closure() = default;

            constexpr explicit action_closure(ActionFn fn): ActionFn(static_cast<ActionFn &&>(fn))
            {}
        };

    } // namespace actions

    template<typename ActionFn>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<actions::action_closure<ActionFn>> = true;
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif