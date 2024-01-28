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
#include <EASTL/string.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        struct getlines_view : view_facade<getlines_view, unknown>
        {
        private:
            friend range_access;
            std::istream * sin_;
            eastl::string str_;
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
                eastl::string & read() const noexcept
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
                if(!getline(*sin_, str_, delim_))
                    sin_ = nullptr;
            }
            cursor begin_cursor()
            {
                return cursor{this};
            }

            // implementation according to
            // https://en.cppreference.com/w/cpp/string/basic_string/getline
            std::istream & getline(std::istream & sin, eastl::string & str,
                                   const char del)
            {
                eastl::string::value_type c;

                str.erase(); // 1) Calls str.erase().

                while(sin.get(
                    c)) // 2) Extracts characters from input and appends them to str until
                        // one of the following occurs (checked in the order listed)
                {
                    if(sin.eof()) // 2a) end-of-file condition on input, in which case,
                                  // getline sets eofbit.
                    {
                        sin.setstate(std::ios_base::eofbit);
                        break;
                    }
                    if(c ==
                       del) // b)The next available input character is delim, as tested by
                            // Traits::eq(c, delim), in which case the delimiter character
                            // is extracted from input, but is not appended to str.
                    {
                        break;
                    }
                    if(str.size() >=
                       str.max_size()) // 2c) str.max_size() characters have been stored,
                                       // in which case getline sets failbit and returns.
                    {
                        sin.setstate(std::ios_base::failbit);
                        break;
                    }
                    else
                        str += c;
                }
                return sin;
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
            eastl::string & cached() noexcept
            {
                return str_;
            }
        };

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
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
