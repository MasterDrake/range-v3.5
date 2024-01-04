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
#ifndef EARANGES_UTILITY_ADDRESSOF_HPP
#define EARANGES_UTILITY_ADDRESSOF_HPP

#include <EASTL/memory.h>

#include <EASTL/type_traits.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/detail/config.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
#if EASTL_ADDRESSOF_CONSTEXPR
        using eastl::addressof;
        //TODO:#error Teoricamente il compiler ha implementato addressof come constexpr ma eastl no, quindi potremmo usare questo switch per vedere se funzionano i constexpr tests
#else
        namespace check_addressof
        {
            inline ignore_t operator&(ignore_t)
            {
                return {};
            }
            template<typename T>
            auto addressof(T & t)
            {
                return &t;
            }
        } // namespace check_addressof

        template<typename T>
        constexpr bool has_bad_addressof()
        {
            return !eastl::is_scalar<T>::value && !EARANGES_IS_SAME(decltype(check_addressof::addressof(*(T *)nullptr)),ignore_t);
        }

        template(typename T)(requires(has_bad_addressof<T>()))
        T * addressof(T & arg) noexcept
        {
            return eastl::addressof(arg);
        }

        template(typename T)(requires (!has_bad_addressof<T>()))
        constexpr T * addressof(T & arg) noexcept
        {
            return &arg;
        }

        template<typename T>
        T const * addressof(T const &&) = delete;
#endif
    } // namespace detail
    /// \endcond
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif