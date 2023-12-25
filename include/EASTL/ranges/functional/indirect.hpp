
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
#ifndef RANGES_V3_FUNCTIONAL_INDIRECT_HPP
#define RANGES_V3_FUNCTIONAL_INDIRECT_HPP

#include "EASTL/utility.h"

#include "../concepts/concepts.hpp"

#include "../functional/invoke.hpp"
#include "../iterator/traits.hpp"
#include "../utility/move.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    template<typename Fn>
    struct indirected
    {
    private:
        RANGES_NO_UNIQUE_ADDRESS
        Fn fn_;

    public:
        indirected() = default;
        indirected(Fn fn)
          : fn_(eastl::move(fn))
        {}
        // value_type (needs no impl)
        template<typename... Its>
        [[noreturn]] invoke_result_t<Fn &, iter_reference_t<Its>...> //
        operator()(copy_tag, Its...) const
        {
            RANGES_EXPECT(false);
        }

        // Reference
        // clang-format off
        template<typename... Its>
        auto CPP_auto_fun(operator())(Its... its)
        (
            return invoke(fn_, *its...)
        )
        template<typename... Its>
        auto CPP_auto_fun(operator())(Its... its)(const)
        (
            return invoke((Fn const &)fn_, *its...)
        )

        // Rvalue reference
        template<typename... Its>
        auto CPP_auto_fun(operator())(move_tag, Its... its)
        (
            return static_cast<
                aux::move_t<invoke_result_t<Fn &, iter_reference_t<Its>...>>>(
                aux::move(invoke(fn_, *its...)))
        )
        template<typename... Its>
        auto CPP_auto_fun(operator())(move_tag, Its... its)(const)
        (
            return static_cast<
                aux::move_t<invoke_result_t<Fn const &, iter_reference_t<Its>...>>>(
                aux::move(invoke((Fn const &)fn_, *its...)))
        )
        // clang-format on
    };

    struct indirect_fn
    {
        template<typename Fn>
        constexpr indirected<Fn> operator()(Fn fn) const
        {
            return indirected<Fn>{detail::move(fn)};
        }
    };

    /// \ingroup group-functional
    /// \sa `indirect_fn`
    RANGES_INLINE_VARIABLE(indirect_fn, indirect)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif