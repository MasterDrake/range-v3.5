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

#ifndef RANGES_V3_VIEW_TOKENIZE_HPP
#define RANGES_V3_VIEW_TOKENIZE_HPP

#include "EASTL/initializer_list.h"
#include <regex>
#include "EASTL/type_traits.h"
#include "EASTL/utility.h"
#include "EASTL/vector.h"

#include "../range_fwd.hpp"

#include "../functional/bind_back.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../utility/static_const.hpp"
#include "../view/all.hpp"
#include "../view/interface.hpp"
#include "../view/view.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Regex, typename SubMatchRange>
    struct tokenize_view
      : view_interface<tokenize_view<Rng, Regex, SubMatchRange>,
                       is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        CPP_assert(bidirectional_range<Rng> && view_<Rng> && common_range<Rng>);
        CPP_assert(semiregular<Regex>);
        CPP_assert(semiregular<SubMatchRange>);

        Rng rng_;
        Regex rex_;
        SubMatchRange subs_;
        std::regex_constants::match_flag_type flags_;
        template<bool Const>
        using iterator_t = std::regex_token_iterator<iterator_t<meta::const_if_c<Const, Rng>>>;

    public:
        tokenize_view() = default;
        tokenize_view(Rng rng, Regex rex, SubMatchRange subs,
                      std::regex_constants::match_flag_type flags)
          : rng_(eastl::move(rng))
          , rex_(eastl::move(rex))
          , subs_(eastl::move(subs))
          , flags_(flags)
        {}
        iterator_t<simple_view<Rng>()> begin()
        {
            meta::const_if_c<simple_view<Rng>(), Rng> & rng = rng_;
            return {ranges::begin(rng), ranges::end(rng), rex_, subs_, flags_};
        }
        template(bool Const = true)(requires range<Rng const>)
        iterator_t<Const> begin() const
        {
            return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
        }
        iterator_t<simple_view<Rng>()> end()
        {
            return {};
        }
        template(bool Const = true)(requires range<Rng const>)
        iterator_t<Const> end() const
        {
            return {};
        }
        Rng base() const
        {
            return rng_;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template(typename Rng, typename Regex, typename SubMatchRange)(
        requires copy_constructible<Regex> AND copy_constructible<SubMatchRange>)
        tokenize_view(Rng &&, Regex, SubMatchRange)  -> tokenize_view<views::all_t<Rng>, Regex, SubMatchRange>;
#endif

    namespace views
    {
        struct tokenize_base_fn
        {
            template(typename Rng, typename Regex)(
                requires bidirectional_range<Rng> AND common_range<Rng> AND
                    same_as< //
                        range_value_t<Rng>, //
                        typename detail::decay_t<Regex>::value_type>)
            tokenize_view<all_t<Rng>, detail::decay_t<Regex>, int> //
            operator()(Rng && rng,
                       Regex && rex,
                       int sub = 0,
                       std::regex_constants::match_flag_type flags =
                            std::regex_constants::match_default) const //
            {
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        sub,
                        flags};
            }

            template(typename Rng, typename Regex)(
                requires bidirectional_range<Rng> AND common_range<Rng> AND
                    same_as<range_value_t<Rng>,
                            typename detail::decay_t<Regex>::value_type>)
            tokenize_view<all_t<Rng>, detail::decay_t<Regex>, eastl::vector<int>> //
            operator()(Rng && rng,
                       Regex && rex, eastl::vector<int> subs,
                       std::regex_constants::match_flag_type flags =
                           std::regex_constants::match_default) const //
            {
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        eastl::move(subs),
                        flags};
            }

            template(typename Rng, typename Regex)(
                requires bidirectional_range<Rng> AND common_range<Rng> AND
                    same_as<range_value_t<Rng>,
                            typename detail::decay_t<Regex>::value_type>)
            tokenize_view<all_t<Rng>,
                          detail::decay_t<Regex>,
                          std::initializer_list<int>> //
            operator()(Rng && rng,
                       Regex && rex,
                       std::initializer_list<int> subs,
                       std::regex_constants::match_flag_type flags =
                           std::regex_constants::match_default) const //
            {
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        eastl::move(subs),
                        flags};
            }
        };

        struct tokenize_fn : tokenize_base_fn
        {
            using tokenize_base_fn::operator();

            template<typename Regex>
            constexpr auto operator()(Regex && rex,
                                      int sub = 0,
                                      std::regex_constants::match_flag_type flags =
                                          std::regex_constants::match_default) const
            {
                return make_view_closure(bind_back(
                    tokenize_base_fn{}, static_cast<Regex &&>(rex), sub, flags));
            }

            template<typename Regex>
            auto operator()(Regex && rex, eastl::vector<int> subs,
                            std::regex_constants::match_flag_type flags =
                                std::regex_constants::match_default) const
            {
                return bind_back(tokenize_base_fn{},
                                 static_cast<Regex &&>(rex),
                                 eastl::move(subs),
                                 flags);
            }

            template<typename Regex>
            constexpr auto operator()(Regex && rex,
                                      std::initializer_list<int> subs,
                                      std::regex_constants::match_flag_type flags =
                                          std::regex_constants::match_default) const
            {
                return make_view_closure(bind_back(tokenize_base_fn{}, static_cast<Regex &&>(rex), subs, flags));
            }
        };

        /// \relates tokenize_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(tokenize_fn, tokenize)
    } // namespace views
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"
#include "../detail/satisfy_boost_range.hpp"
RANGES_SATISFY_BOOST_RANGE(::ranges::tokenize_view)

#endif