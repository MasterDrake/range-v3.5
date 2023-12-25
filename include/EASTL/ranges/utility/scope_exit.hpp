/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2017-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_SCOPE_EXIT_HPP
#define RANGES_V3_UTILITY_SCOPE_EXIT_HPP


#include "EASTL/functional.h"
#include "EASTL/type_traits.h"
#include "EASTL/utility.h"
//#include <EASTL/functional.h>
//#include <EASTL/type_traits.h>
//#include <EASTL/utility.h>

#include "../meta/meta.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    template<typename Fun>
    struct scope_exit
    {
    private:
        using nothrow_move_t = eastl::is_nothrow_move_constructible<Fun>;
        using nothrow_copy_t = eastl::is_nothrow_copy_constructible<Fun>;

        Fun fun_;
        bool dismissed_{false};

        using ref_t = decltype(eastl::ref(eastl::declval<Fun const &>()));
        using guard = scope_exit<ref_t>;

        scope_exit(Fun const & fun, guard && g)
          : fun_(fun)
        {
            g.dismiss();
        }

        scope_exit(Fun const & fun, eastl::false_type)
          : scope_exit(fun, guard{eastl::ref(fun)})
        {}

        scope_exit(Fun const & fun, eastl::true_type) noexcept
          : fun_(fun)
        {}

        scope_exit(Fun && fun, eastl::true_type) noexcept
          : fun_(eastl::move(fun))
        {}

    public:
        explicit scope_exit(Fun const & fun) noexcept(
            noexcept(scope_exit(fun, nothrow_copy_t{})))
          : scope_exit(fun, nothrow_copy_t{})
        {}

        explicit scope_exit(Fun && fun) noexcept(noexcept(scope_exit(eastl::move(fun), nothrow_move_t{})))
          : scope_exit(eastl::move(fun), nothrow_move_t{})
        {}

        scope_exit(scope_exit const &) = delete;

        scope_exit(scope_exit && that) noexcept(eastl::is_nothrow_move_constructible<Fun>::value) : scope_exit(eastl::move((that.dismiss(), that)).fun_)
        {}

        ~scope_exit()
        {
            if(!dismissed_)
                fun_();
        }

        void dismiss() noexcept
        {
            dismissed_ = true;
        }
    };

    template<typename Fun, typename ScopeExit = scope_exit<meta::_t<eastl::decay<Fun>>>>
    ScopeExit make_scope_exit(Fun && fun) noexcept(noexcept(ScopeExit(ScopeExit((Fun &&) fun))))
    {
        return ScopeExit((Fun &&) fun);
    }
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif