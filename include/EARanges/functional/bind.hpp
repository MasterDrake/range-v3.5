
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
#ifndef EARANGES_FUNCTIONAL_BIND_HPP
#define EARANGES_FUNCTIONAL_BIND_HPP

#include <EASTL/type_traits.h>
#include <EASTL/functional.h>
//TODO:: FUNCTIONAL qui e' un big problem per std::BIND -.--.--.-
#include <functional>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-functional
        /// @{
        template<
            typename T,
            typename U = meta::if_<
                eastl::is_lvalue_reference<T>,
                eastl::reference_wrapper<meta::_t<eastl::remove_reference<T>>>, T &&>>
        U bind_forward(meta::_t<eastl::remove_reference<T>> & t) noexcept
        {
            return static_cast<U>(t);
        }

        template<typename T>
        T && bind_forward(meta::_t<eastl::remove_reference<T>> && t) noexcept
        {
            // This is to catch way sketchy stuff like: forward<int const &>(42)
            static_assert(!eastl::is_lvalue_reference<T>::value,
                          "You didn't just do that!");
            return static_cast<T &&>(t);
        }

        template<typename T>
        struct bind_element
          : meta::if_c<EARANGES_IS_SAME(detail::decay_t<T>, T), meta::id<T>,
                       bind_element<detail::decay_t<T>>>
        {};

        template<typename T>
        struct bind_element<eastl::reference_wrapper<T>>
        {
            using type = T &;
        };

        template<typename T>
        struct bind_element<reference_wrapper<T>>
        {
            using type = typename reference_wrapper<T>::reference;
        };

        template<typename T>
        using bind_element_t = meta::_t<bind_element<T>>;

        template<typename Bind>
        struct protector
        {
        private:
            Bind bind_;

        public:
            protector() = default;
            protector(Bind b)
              : bind_(eastl::move(b))
            {}
            // clang-format off
        template<typename...Ts>
        auto CPP_auto_fun(operator())(Ts &&...ts)
        (
            return bind_(static_cast<Ts &&>(ts)...)
        )
        /// \overload
        template<typename...Ts>
        auto CPP_auto_fun(operator())(Ts &&...ts) (const)
        (
            return bind_(static_cast<Ts &&>(ts)...)
        )
            // clang-format on
        };

        // TODO: OH OH BIG PROBLEM -> no eastl::is_bind_expression and no eastl::bind :O
        // Rimuovere il requirement?
        struct protect_fn
        {
            template(typename F)(requires std::is_bind_expression<uncvref_t<F>>::value) //
                protector<uncvref_t<F>>
                operator()(F && f) const
            {
                return {static_cast<F &&>(f)};
            }
            /// \overload
            template(typename F)(
                requires(!std::is_bind_expression<uncvref_t<F>>::value)) //
                F
                operator()(F && f) const
            {
                return static_cast<F &&>(f);
            }
        };

        /// Protect a callable so that it can be safely used in a bind expression without
        /// accidentally becoming a "nested" bind.
        /// \ingroup group-functional
        /// \sa `protect_fn`
        EARANGES_INLINE_VARIABLE(protect_fn, protect)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
