// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_DETAIL_SATISFY_BOOST_RANGE_HPP
#define EARANGES_DETAIL_SATISFY_BOOST_RANGE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>


//TODO: Could be a way to make range-v4 std compatible, so keep it
#ifndef EARANGES_DOXYGEN_INVOKED
namespace eastl
{
    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3
    /// views
    template<typename T, typename U>
    struct range_mutable_iterator;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3
    /// views
    template<typename T, typename U>
    struct range_const_iterator;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3
    /// views
    template<typename T>
    struct range_value;

    /// \brief Boost Range specialisation point, for making Boost ranges out of range-v3
    /// views
    template<typename T>
    struct range_size;
} // namespace boost

//TODO: Forse non dovrebbe servire questo eastl, ma meglio di niente, ripetita iuvant, altrimenti ritorniamo a boost e usiamo std per una migliore compatibilita'
/// \brief Macro specialising Boost Range metafunctions for the specified view
#define EARANGES_SATISFY_BOOST_RANGE(view_name)                                           \
    namespace eastl                                                                     \
    {                                                                                   \
        template<typename... Ts>                                                        \
        struct range_mutable_iterator<                                                  \
            view_name<Ts...>,                                                           \
            ::meta::if_c<(bool)::ranges::common_range<view_name<Ts...>>>>               \
        {                                                                               \
            using type = ::ranges::iterator_t<view_name<Ts...>>;                        \
        };                                                                              \
        template<typename... Ts>                                                        \
        struct range_const_iterator<                                                    \
            view_name<Ts...>,                                                           \
            ::meta::if_c<(bool)::ranges::common_range<view_name<Ts...> const>>>         \
        {                                                                               \
            using type = ::ranges::iterator_t<view_name<Ts...> const>;                  \
        };                                                                              \
        template<typename... Ts>                                                        \
        struct range_value<view_name<Ts...>>                                            \
        {                                                                               \
            using type = ::ranges::range_value_t<view_name<Ts...>>;                     \
        };                                                                              \
        template<typename... Ts>                                                        \
        struct range_size<view_name<Ts...>>                                             \
          : ::meta::if_c<(bool)::ranges::common_range<view_name<Ts...>>,                \
                         ::meta::defer<::ranges::range_size_t, view_name<Ts...>>,       \
                         ::meta::nil_>                                                  \
        {};                                                                             \
        template<typename... Ts>                                                        \
        struct range_size<view_name<Ts...> const>                                       \
          : ::meta::if_c<(bool)::ranges::common_range<view_name<Ts...> const>,          \
                         ::meta::defer<::ranges::range_size_t, view_name<Ts...> const>, \
                         ::meta::nil_>                                                  \
        {};                                                                             \
    }
#else
#define EARANGES_SATISFY_BOOST_RANGE(view_name)
#endif

#endif
