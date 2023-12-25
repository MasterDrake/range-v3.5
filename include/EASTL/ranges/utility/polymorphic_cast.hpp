//  (C) Copyright Kevlin Henney and Dave Abrahams 1999.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP
#define RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP

//TODO: A cosa serve? Solo per addresof? #include <memory>
//#include <EASTL/type_traits.h>
#include "EASTL/type_traits.h"
#include "EASTL/memory.h"

#include "../meta/meta.hpp"

#include "../detail/config.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    template<typename Target, typename Source>
    auto polymorphic_downcast(Source * x) noexcept
        -> meta::if_<eastl::is_pointer<Target>,
                     decltype((static_cast<Target>(x), dynamic_cast<Target>(x)))>
    {
        auto result = static_cast<Target>(x);
        RANGES_ASSERT(dynamic_cast<Target>(x) == result);
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
        RANGES_ASSERT(eastl::addressof(dresult) == eastl::addressof(result));
#endif
        return static_cast<Target>(result);
    }
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP