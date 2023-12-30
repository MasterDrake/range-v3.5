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
#ifndef EARANGES_FUNCTIONAL_IDENTITY_HPP
#define EARANGES_FUNCTIONAL_IDENTITY_HPP

#include <EARanges/detail/config.hpp>
         
#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    struct identity
    {
        template<typename T>
        constexpr T && operator()(T && t) const noexcept
        {
            return (T &&) t;
        }
        using is_transparent = void;
    };

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif