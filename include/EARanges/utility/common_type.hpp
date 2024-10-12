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

#ifndef EARANGES_UTILITY_COMMON_TYPE_HPP
#define EARANGES_UTILITY_COMMON_TYPE_HPP

#include <EASTL/tuple.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/type_traits.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/detail/prologue.hpp>

// Sadly, this is necessary because of:
//  - std::common_type is !SFINAE-friendly, and
//  - The specification of std::common_type makes it impossibly
//    difficult to specialize on user-defined types without spamming
//    out a bajillion copies to handle all combinations of cv and ref
//    qualifiers.

namespace ranges
{
    template<typename... Ts>
    using common_type = concepts::common_type<Ts...>;

    template<typename... Ts>
    using common_type_t = concepts::common_type_t<Ts...>;

    template<typename... Ts>
    using common_reference = concepts::common_reference<Ts...>;

    template<typename... Ts>
    using common_reference_t = concepts::common_reference_t<Ts...>;

    /// \cond
    template<typename F, typename S>
    struct common_pair;

    template<typename... Ts>
    struct common_tuple;
    /// \endcond
} // namespace ranges

/// \cond
// Specializations for pair and tuple
namespace concepts
{
    // common_type for eastl::pairs
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<eastl::pair<F1, S1>, ::ranges::common_pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<::ranges::common_pair<F1, S1>, eastl::pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<::ranges::common_pair<F1, S1>, ::ranges::common_pair<F2, S2>>;

    // common_type for eastl::tuples
    template<typename... Ts, typename... Us>
    struct common_type<::ranges::common_tuple<Ts...>, eastl::tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<eastl::tuple<Ts...>, ::ranges::common_tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<::ranges::common_tuple<Ts...>, ::ranges::common_tuple<Us...>>;

    // A common reference for eastl::pairs
    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_pair<F1, S1>, eastl::pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<eastl::pair<F1, S1>, ::ranges::common_pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_pair<F1, S1>, ::ranges::common_pair<F2, S2>, Qual1, Qual2>;

    // A common reference for eastl::tuples
    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_tuple<Ts...>, eastl::tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<eastl::tuple<Ts...>, ::ranges::common_tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_tuple<Ts...>, ::ranges::common_tuple<Us...>, Qual1, Qual2>;
} // namespace concepts

#if EARANGES_CXX_VER > EARANGES_CXX_STD_17
EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
EARANGES_BEGIN_NAMESPACE_STD
EARANGES_BEGIN_NAMESPACE_VERSION

    template<typename...>
    struct common_type;

    // common_type for eastl::pairs
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<eastl::pair<F1, S1>, ::ranges::common_pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<::ranges::common_pair<F1, S1>, eastl::pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<::ranges::common_pair<F1, S1>, ::ranges::common_pair<F2, S2>>;

    // common_type for eastl::tuples
    template<typename... Ts, typename... Us>
    struct common_type<::ranges::common_tuple<Ts...>, eastl::tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<eastl::tuple<Ts...>, ::ranges::common_tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<::ranges::common_tuple<Ts...>, ::ranges::common_tuple<Us...>>;

    template<typename, typename, template<typename> class, template<typename> class>
    struct basic_common_reference;

    // A common reference for eastl::pairs
    template<typename F1, typename S1, typename F2, typename S2, template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_pair<F1, S1>, eastl::pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<eastl::pair<F1, S1>, ::ranges::common_pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_pair<F1, S1>, ::ranges::common_pair<F2, S2>, Qual1, Qual2>;

    // A common reference for eastl::tuples
    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_tuple<Ts...>, eastl::tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<eastl::tuple<Ts...>, ::ranges::common_tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<::ranges::common_tuple<Ts...>, ::ranges::common_tuple<Us...>, Qual1, Qual2>;

EARANGES_END_NAMESPACE_VERSION
EARANGES_END_NAMESPACE_STD
EARANGES_DIAGNOSTIC_POP
#endif // EARANGES_CXX_VER > EARANGES_CXX_STD_17
/// \endcond

#include <EARanges/detail/epilogue.hpp>

#endif
