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

#ifndef EARANGES_UTILITY_TAGGED_TUPLE_HPP
#define EARANGES_UTILITY_TAGGED_TUPLE_HPP

#include <EASTL/tuple.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/tagged_pair.hpp>

#include <EARanges/detail/prologue.hpp>

EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

//TODO:Capire se qualcuno o quacosa sta ancora usando cio e rimuoverlo.
namespace ranges
{
    template<typename... Ts>
    using tagged_tuple EARANGES_DEPRECATED("ranges::tagged_tuple is deprecated.") = tagged<eastl::tuple<detail::tag_elem<Ts>...>, detail::tag_spec<Ts>...>;

    template<typename... Tags, typename... Ts>
    EARANGES_DEPRECATED("ranges::make_tagged_tuple is deprecated.")
    constexpr tagged_tuple<Tags(bind_element_t<Ts>)...> make_tagged_tuple(Ts &&... ts)
    {
        return tagged_tuple<Tags(bind_element_t<Ts>)...>{static_cast<Ts &&>(ts)...};
    }
} // namespace ranges

EARANGES_DIAGNOSTIC_POP

#include <EARanges/detail/epilogue.hpp>

#endif