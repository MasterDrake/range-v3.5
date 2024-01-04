//  (C) Copyright Kevlin Henney and Dave Abrahams 1999.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EARANGES_UTILITY_POLYMORPHIC_CAST_HPP
#define EARANGES_UTILITY_POLYMORPHIC_CAST_HPP

#include <EASTL/type_traits.h>
#include <EASTL/memory.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/detail/config.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    template<typename Target, typename Source>
    auto polymorphic_downcast(Source * x) noexcept
        -> meta::if_<eastl::is_pointer<Target>,
                     decltype((static_cast<Target>(x), dynamic_cast<Target>(x)))>
    {
        auto result = static_cast<Target>(x);
        EARANGES_ASSERT(dynamic_cast<Target>(x) == result);
        return result;
    }
    template<typename Target, typename Source>
    auto polymorphic_downcast(Source && x) noexcept
        -> meta::if_<eastl::is_reference<Target>,
                     decltype((static_cast<Target>(eastl::declval<Source>()),
                               dynamic_cast<Target>(eastl::declval<Source>())))>
    {
        auto && result = static_cast<Target>(static_cast<Source &&>(x));
#ifndef NDEBUG
        auto && dresult = dynamic_cast<Target>(static_cast<Source &&>(x));
        EARANGES_ASSERT(eastl::addressof(dresult) == eastl::addressof(result));
#endif
        return static_cast<Target>(result);
    }
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_UTILITY_POLYMORPHIC_CAST_HPP