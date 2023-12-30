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

#ifndef EARANGES_VIEW_GETLINES_HPP
#define EARANGES_VIEW_GETLINES_HPP

#include <istream>
#include <string>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    struct getlines_view : view_facade<getlines_view, unknown>
    {
    private:
        friend range_access;
        std::istream * sin_;
        std::string str_;//TODO: Fix this string to eastl string and see if it's worth it, maybe yes because we're porting the ranges library and an iterator line this would be cool
        char delim_;
        struct cursor
        {
        private:
            friend range_access;
            using single_pass = eastl::true_type;
            getlines_view * rng_ = nullptr;

        public:
            cursor() = default;
            explicit cursor(getlines_view * rng)
              : rng_(rng)
            {}
            void next()
            {
                rng_->next();
            }
            std::string & read() const noexcept
            {
                return rng_->str_;
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
            if(!std::getline(*sin_, str_, delim_))
                sin_ = nullptr;
        }
        cursor begin_cursor()
        {
            return cursor{this};
        }

    public:
        getlines_view() = default;
        getlines_view(std::istream & sin, char delim = '\n')
          : sin_(&sin)
          , str_{}
          , delim_(delim)
        {
            this->next(); // prime the pump
        }
        std::string & cached() noexcept
        {
            return str_;
        }
    };

    /// \cond
    using getlines_range EARANGES_DEPRECATED(
        "getlines_range has been renamed getlines_view") = getlines_view;
    /// \endcond

    struct getlines_fn
    {
        getlines_view operator()(std::istream & sin, char delim = '\n') const
        {
            return getlines_view{sin, delim};
        }
    };

    EARANGES_INLINE_VARIABLE(getlines_fn, getlines)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif