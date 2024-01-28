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

#ifndef EARANGES_VIEW_REPEAT_HPP
#define EARANGES_VIEW_REPEAT_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/unreachable_sentinel.hpp>
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
        // repeat_view *does* own its lone element, though. This is OK because:
        //  - O(N) copying is fine when N==1 as it is in this case, and
        //  - The element is immutable, so there is no potential for incorrect
        //    semantics.
        template<typename Val>
        struct repeat_view : view_facade<repeat_view<Val>, infinite>
        {
        private:
            semiregular_box_t<Val> value_;
            friend range_access;

            struct cursor
            {
            private:
                Val const * value_;
                std::ptrdiff_t n_ = 0;

            public:
                cursor() = default;
                explicit cursor(Val const & value)
                  : value_(eastl::addressof(value))
                {}
                Val const & read() const noexcept
                {
                    return *value_;
                }
                bool equal(cursor const & that) const
                {
                    return n_ == that.n_;
                }
                void next()
                {
                    ++n_;
                }
                void prev()
                {
                    --n_;
                }
                void advance(std::ptrdiff_t d)
                {
                    n_ += d;
                }
                std::ptrdiff_t distance_to(cursor const & that) const
                {
                    return that.n_ - n_;
                }
            };
            cursor begin_cursor() const
            {
                return cursor{value_};
            }
            unreachable_sentinel_t end_cursor() const
            {
                return unreachable;
            }

        public:
            repeat_view() = default;
            constexpr explicit repeat_view(Val value)
              : value_(detail::move(value))
            {}
        };

        namespace views
        {
            struct repeat_fn
            {
                template(typename Val)(requires copy_constructible<Val>) repeat_view<Val>
                operator()(Val value) const
                {
                    return repeat_view<Val>{eastl::move(value)};
                }
            };

            /// \relates repeat_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(repeat_fn, repeat)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>
#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::repeat_view)

#endif