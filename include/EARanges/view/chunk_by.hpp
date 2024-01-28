/// \file
// Range v3 library
//
//  Copyright Hui Xie 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_CHUNK_BY_HPP
#define EARANGES_VIEW_CHUNK_BY_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/adjacent_find.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/not_fn.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/optional.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/facade.hpp>
#include <EARanges/view/subrange.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct chunk_by_view
          : view_facade<chunk_by_view<Rng, Fun>,
                        is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            Rng rng_;
            // cached version of the end of the first subrange / start of the second
            // subrange
            detail::non_propagating_cache<iterator_t<Rng>> second_;
            semiregular_box_t<Fun> fun_;

            struct cursor
            {
            private:
                friend range_access;
                friend chunk_by_view;
                iterator_t<Rng> cur_;
                iterator_t<Rng> next_cur_;
                sentinel_t<Rng> last_;
                semiregular_box_ref_or_val_t<Fun, false> fun_;

#ifdef _MSC_VER
                template<typename I = iterator_t<Rng>>
                subrange<I> read() const
                {
                    return {cur_, next_cur_};
                }
#else
                subrange<iterator_t<Rng>> read() const
                {
                    return {cur_, next_cur_};
                }
#endif
                void next()
                {
                    cur_ = next_cur_;
                    auto partition_cur = adjacent_find(cur_, last_, not_fn(fun_));
                    next_cur_ = partition_cur != last_ ? ranges::next(partition_cur)
                                                       : partition_cur;
                }

                bool equal(default_sentinel_t) const
                {
                    return cur_ == last_;
                }
                bool equal(cursor const & that) const
                {
                    return cur_ == that.cur_;
                }
                cursor(semiregular_box_ref_or_val_t<Fun, false> fun,
                       iterator_t<Rng> first, iterator_t<Rng> next_cur,
                       sentinel_t<Rng> last)
                  : cur_(first)
                  , next_cur_(next_cur)
                  , last_(last)
                  , fun_(fun)
                {}

            public:
                cursor() = default;
            };
            cursor begin_cursor()
            {
                auto first = ranges::begin(rng_);
                auto last = ranges::end(rng_);
                if(!second_)
                {
                    auto partition_cur = adjacent_find(first, last, not_fn(fun_));
                    second_ = partition_cur != last ? ranges::next(partition_cur)
                                                    : partition_cur;
                }
                return {fun_, first, *second_, last};
            }

        public:
            chunk_by_view() = default;
            constexpr chunk_by_view(Rng rng, Fun fun)
              : rng_(eastl::move(rng))
              , fun_(eastl::move(fun))
            {}
        };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename Rng, typename Fun)(requires copy_constructible<Fun>)
            chunk_by_view(Rng &&, Fun) -> chunk_by_view<views::all_t<Rng>, Fun>;
#endif

        namespace views
        {
            struct chunk_by_base_fn
            {
                template(typename Rng, typename Fun)(
                    requires viewable_range<Rng> AND forward_range<Rng> AND //
                        indirect_relation<Fun, iterator_t<Rng>>)            //
                    constexpr chunk_by_view<all_t<Rng>, Fun>
                    operator()(Rng && rng, Fun fun) const
                {
                    return {all(static_cast<Rng &&>(rng)), eastl::move(fun)};
                }
            };

            struct chunk_by_fn : chunk_by_base_fn
            {
                using chunk_by_base_fn::operator();

                template<typename Fun>
                constexpr auto operator()(Fun fun) const
                {
                    return make_view_closure(
                        bind_back(chunk_by_base_fn{}, eastl::move(fun)));
                }
            };

            /// \relates chunk_by_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(chunk_by_fn, chunk_by)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::chunk_by_view)

#endif
