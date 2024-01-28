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

#ifndef EARANGES_VIEW_ISTREAM_HPP
#define EARANGES_VIEW_ISTREAM_HPP

#include <istream>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
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
        template<typename Val>
        struct istream_view : view_facade<istream_view<Val>, unknown>
        {
        private:
            friend range_access;
            std::istream * sin_;
            semiregular_box_t<Val> obj_;
            struct cursor
            {
            private:
                friend range_access;
                using single_pass = eastl::true_type;
                istream_view * rng_ = nullptr;

            public:
                cursor() = default;
                explicit cursor(istream_view * rng)
                  : rng_(rng)
                {}
                void next()
                {
                    rng_->next();
                }
                Val & read() const noexcept
                {
                    return rng_->cached();
                }
                bool equal(default_sentinel_t) const
                {
                    return !rng_->sin_;
                }
                bool equal(cursor that) const
                {
                    return !rng_->sin_ == !that.rng_->sin_;
                }
            };
            void next()
            {
                if(!(*sin_ >> cached()))
                    sin_ = nullptr;
            }
            cursor begin_cursor()
            {
                return cursor{this};
            }

        public:
            istream_view() = default;
            explicit istream_view(std::istream & sin)
              : sin_(&sin)
              , obj_{}
            {
                next(); // prime the pump
            }
            Val & cached() noexcept
            {
                return obj_;
            }
        };

        /// \cond
        namespace _istream_
        {
            /// \endcond
            template(typename Val)(
                requires copy_constructible<Val> AND default_constructible<
                    Val>) inline istream_view<Val> istream(std::istream & sin)
            {
                return istream_view<Val>{sin};
            }
            /// \cond
        } // namespace _istream_
        using namespace _istream_;
        /// \endcond

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
