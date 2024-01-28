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

#ifndef EARANGES_VIEW_REPEAT_N_HPP
#define EARANGES_VIEW_REPEAT_N_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/semiregular_box.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{

        // Ordinarily, a view shouldn't contain its elements. This is so that copying
        // and assigning ranges is O(1), and also so that in the event of element
        // mutation, all the copies of the range see the mutation the same way. The
        // repeat_n_view *does* own its lone element, though. This is OK because:
        //  - O(N) copying is fine when N==1 as it is in this case, and
        //  - The element is immutable, so there is no potential for incorrect
        //    semantics.
        template<typename Val>
        struct repeat_n_view : view_facade<repeat_n_view<Val>, finite>
        {
        private:
            friend range_access;
            semiregular_box_t<Val> value_;
            std::ptrdiff_t n_;

            struct cursor
            {
            private:
                Val const * value_;
                std::ptrdiff_t n_;

            public:
                cursor() = default;
                cursor(Val const & value, std::ptrdiff_t n)
                  : value_(eastl::addressof(value))
                  , n_(n)
                {}
                Val const & read() const
                {
                    return *value_;
                }
                constexpr bool equal(default_sentinel_t) const
                {
                    return 0 == n_;
                }
                bool equal(cursor const & that) const
                {
                    return n_ == that.n_;
                }
                void next()
                {
                    EARANGES_EXPECT(0 != n_);
                    --n_;
                }
                void prev()
                {
                    ++n_;
                }
                void advance(std::ptrdiff_t n)
                {
                    n_ -= n;
                }
                std::ptrdiff_t distance_to(cursor const & that) const
                {
                    return n_ - that.n_;
                }
            };
            cursor begin_cursor() const
            {
                return {value_, n_};
            }

        public:
            repeat_n_view() = default;
            constexpr repeat_n_view(Val value, std::ptrdiff_t n)
              : value_(detail::move(value))
              , n_((EARANGES_EXPECT(0 <= n), n))
            {}
            constexpr std::size_t size() const
            {
                return static_cast<std::size_t>(n_);
            }
        };

        namespace views
        {
            struct repeat_n_fn
            {
                template(typename Val)(requires copy_constructible<Val>)
                    repeat_n_view<Val>
                    operator()(Val value, std::ptrdiff_t n) const
                {
                    return repeat_n_view<Val>{eastl::move(value), n};
                }
            };

            /// \relates repeat_n_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(repeat_n_fn, repeat_n)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::repeat_n_view)

#endif
