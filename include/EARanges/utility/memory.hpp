/// \file
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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef EARANGES_UTILITY_MEMORY_HPP
#define EARANGES_UTILITY_MEMORY_HPP

#include <cstdint>

#include <EASTL/type_traits.h>
#include "EASTL/unique_ptr.h"
#include <EASTL/memory.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/detail/config.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/polymorphic_cast.hpp>

#include <EARanges/detail/prologue.hpp>
//TODO:: ALLOCATOR ALLOCATOR ALLOCATOR
namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T>
        eastl::pair<T *, std::ptrdiff_t> get_temporary_buffer_impl(std::size_t n) noexcept
        {
            if(n > PTRDIFF_MAX / sizeof(T))
                n = PTRDIFF_MAX / sizeof(T);

            void * ptr = nullptr;
            for(; ptr == nullptr && n > 0; n /= 2)
            {
#if EARANGES_CXX_ALIGNED_NEW < EARANGES_CXX_ALIGNED_NEW_17
                static_assert(alignof(T) <= alignof(std::max_align_t),
                              "Sorry: over-aligned types are supported only with C++17.");
#else  // EARANGES_CXX_ALIGNED_NEW
                if(EARANGES_CONSTEXPR_IF(alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__))
                    ptr = ::operator new(
                        sizeof(T) * n, std::align_val_t{alignof(T)}, std::nothrow);
                else
#endif // EARANGES_CXX_ALIGNED_NEW
                ptr = ::operator new(sizeof(T) * n, std::nothrow);
            }

            return {static_cast<T *>(ptr), static_cast<std::ptrdiff_t>(n)};
        }

        template<typename T, typename D>
        eastl::pair<T *, std::ptrdiff_t> get_temporary_buffer(D count) noexcept
        {
            EARANGES_EXPECT(count >= 0);
            return detail::get_temporary_buffer_impl<T>(static_cast<std::size_t>(count));
        }

        struct return_temporary_buffer
        {
            template<typename T>
            void operator()(T * p) const
            {
#if EARANGES_CXX_ALIGNED_NEW < EARANGES_CXX_ALIGNED_NEW_17
                static_assert(alignof(T) <= alignof(std::max_align_t),
                              "Sorry: over-aligned types are supported only with C++17.");
#else  // EARANGES_CXX_ALIGNED_NEW
                if(EARANGES_CONSTEXPR_IF(alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__))
                    ::operator delete(p, std::align_val_t{alignof(T)});
                else
#endif // EARANGES_CXX_ALIGNED_NEW
                ::operator delete(p);
            }
        };

        template(typename T, typename... Args)(requires (!eastl::is_array<T>::value)) //
        eastl::unique_ptr<T> make_unique(Args &&... args)
        {
            return eastl::unique_ptr<T>{new T(static_cast<Args &&>(args)...)};
        }
    } // namespace detail
    /// \endcond

    /// \addtogroup group-utility
    /// @{
    template<typename O, typename Val>
    struct raw_storage_iterator
    {
    private:
        CPP_assert(output_iterator<O, Val>);
        CPP_assert(eastl::is_lvalue_reference<iter_reference_t<O>>());
        O out_;

    public:
        using difference_type = iter_difference_t<O>;
        raw_storage_iterator() = default;
        explicit raw_storage_iterator(O out)
          : out_(eastl::move(out))
        {}
        raw_storage_iterator & operator*() noexcept
        {
            return *this;
        }
        CPP_member
        auto operator=(Val const & val) //
            -> CPP_ret(raw_storage_iterator &)(
                requires copy_constructible<Val>)
        {
            ::new((void *)eastl::addressof(*out_)) Val(val);
            return *this;
        }
        CPP_member
        auto operator=(Val && val) //
            -> CPP_ret(raw_storage_iterator &)(
                requires move_constructible<Val>)
        {
            ::new((void *)eastl::addressof(*out_)) Val(eastl::move(val));
            return *this;
        }
        raw_storage_iterator & operator++()
        {
            ++out_;
            return *this;
        }
        CPP_member
        auto operator++(int) //
            -> CPP_ret(void)(
                requires (!forward_iterator<O>))
        {
            ++out_;
        }
        CPP_member
        auto operator++(int) //
            -> CPP_ret(raw_storage_iterator)(
                requires forward_iterator<O>)
        {
            auto tmp = *this;
            ++out_;
            return tmp;
        }
        O base() const
        {
            return out_;
        }
    };

    template<typename I>
    struct iterator_wrapper
    {
    private:
        CPP_assert(input_or_output_iterator<I>);
        mutable I * i_ = nullptr;

    public:
        using difference_type = iter_difference_t<I>;
        iterator_wrapper() = default;
        iterator_wrapper(iterator_wrapper const & that)
          : i_(that.i_)
        {
            that.i_ = nullptr;
        }
        iterator_wrapper & operator=(iterator_wrapper const & that)
        {
            i_ = that.i_;
            that.i_ = nullptr;
            return *this;
        }
        iterator_wrapper(I & i)
          : i_(eastl::addressof(i))
        {}
        // clang-format off
        auto CPP_auto_fun(operator*)()(const)
        (
            return **i_
        )
            // clang-format on
            iterator_wrapper &
            operator++()
        {
            ++*i_;
            return *this;
        }
        void operator++(int)
        {
            ++*i_;
        }
        I base() const
        {
            return *i_;
        }
    };

    template(typename I)(requires input_or_output_iterator<I>)
    iterator_wrapper<I> iter_ref(I & i)
    {
        return i;
    }

    template<typename I>
    struct indirectly_readable_traits<iterator_wrapper<I>>
      : meta::if_c<(bool)input_iterator<I>, indirectly_readable_traits<I>, meta::nil_>
    {};

    template<typename Val>
    struct raw_buffer
    {
    private:
        Val * begin_;
        raw_storage_iterator<Val *, Val> rsi_;

    public:
        explicit raw_buffer(Val * first)
          : begin_(first)
          , rsi_(first)
        {}
        raw_buffer(raw_buffer &&) = default;
        raw_buffer(raw_buffer const &) = delete;
        ~raw_buffer()
        {
            for(; begin_ != rsi_.base(); ++begin_)
                begin_->~Val();
        }
        iterator_wrapper<raw_storage_iterator<Val *, Val>> begin()
        {
            return ranges::iter_ref(rsi_);
        }
    };

    template<typename Val>
    raw_buffer<Val> make_raw_buffer(Val * val)
    {
        return raw_buffer<Val>(val);
    }
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif