// Range v3 library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_TEST_DEBUG_VIEW_HPP
#define EARANGES_TEST_DEBUG_VIEW_HPP

#include <cstddef>
#include <atomic>
#include <EASTL/memory.h>
#include <EASTL/shared_ptr.h>

#include <EARanges/iterator/operations.hpp>
#include <EARanges/utility/swap.hpp>

template<typename T, bool Sized = true>
struct debug_input_view : ranges::view_base
{
    static_assert(eastl::is_object<T>::value, "");

    using index_t = std::ptrdiff_t;
    using version_t = long;

    struct data
    {
        T *const first_;
        index_t const n_;
        std::atomic<index_t> offset_ {-1};

        data(T *p, index_t n)
          : first_(p), n_(n)
        {
            EARANGES_ENSURE(n >= 0);
            EARANGES_ENSURE(p || !n);
        }
    };
    eastl::shared_ptr<data> data_{};
    version_t version_ = 0;

    debug_input_view() = default;
    debug_input_view(T* p, index_t size)
      : data_(eastl::make_shared<data>(p, size))
    {}
    template<index_t N>
    debug_input_view(T (&data)[N])
      : debug_input_view{data, N}
    {}
    debug_input_view(debug_input_view const &that) = default;
    debug_input_view &operator=(debug_input_view const &that)
    {
        data_ = that.data_;
        ++version_; // invalidate outstanding iterators
        return *this;
    }

    struct sentinel
    {
        debug_input_view *view_ = nullptr;
        version_t version_ = 0;

        sentinel() = default;
        explicit constexpr sentinel(debug_input_view &view) noexcept
          : view_{&view}, version_{view.version_}
        {}
    };
    struct iterator
    {
        using iterator_category = eastl::input_iterator_tag;
        using value_type = meta::_t<eastl::remove_cv<T>>;
        using difference_type = index_t;
        using reference = T &;
        using pointer = T *;

        debug_input_view *view_ = nullptr;
        version_t version_ = 0;
        index_t offset_ = -1;

        iterator() = default;
        explicit constexpr iterator(debug_input_view &view) noexcept
          : view_{&view}, version_{view.version_}
          , offset_{view.data_ ? view.data_->offset_.load() : -1}
        {}

        void check_current() const noexcept
        {
            EARANGES_ENSURE(view_);
            EARANGES_ENSURE(view_->version_ == version_);
            EARANGES_ENSURE(view_->data_);
            EARANGES_ENSURE(view_->data_->offset_ == offset_);
        }

        void check_dereferenceable() const noexcept
        {
            check_current();
            EARANGES_ENSURE(view_->data_->offset_ < view_->data_->n_);
        }

        reference operator*() const noexcept
        {
            check_dereferenceable();
            return view_->data_->first_[offset_];
        }
        iterator &operator++() noexcept
        {
            check_dereferenceable();
            EARANGES_ENSURE(view_->data_->offset_.compare_exchange_strong(offset_, offset_ + 1));
            ++offset_;
            return *this;
        }
        void operator++(int) noexcept
        {
            ++*this;
        }

        friend bool operator==(iterator const &i, sentinel const &s)
        {
            EARANGES_ENSURE(i.view_ == s.view_);
            EARANGES_ENSURE(i.version_ == s.version_);
            i.check_current();
            return i.offset_ == i.view_->data_->n_;
        }
        friend bool operator==(sentinel const &s, iterator const &i)
        {
            return i == s;
        }
        friend bool operator!=(iterator const &i, sentinel const &s)
        {
            return !(i == s);
        }
        friend bool operator!=(sentinel const &s, iterator const &i)
        {
            return !(i == s);
        }
        CPP_member
        friend auto operator-(sentinel const& s, iterator const& i) ->
            CPP_ret(difference_type)(
                requires Sized)
        {
            EARANGES_ENSURE(i.view_ == s.view_);
            EARANGES_ENSURE(i.version_ == s.version_);
            i.check_current();
            return i.view_->data_->n_ - i.offset_;
        }
        CPP_member
        friend auto operator-(iterator const& i, sentinel const& s) ->
            CPP_ret(difference_type)(
                requires Sized)
        {
            return -(s - i);
        }
    };
    iterator begin() noexcept
    {
        EARANGES_ENSURE(data_);
        index_t tmp = -1;
        EARANGES_ENSURE(data_->offset_.compare_exchange_strong(tmp, 0));
        return iterator{*this};
    }
    sentinel end() noexcept
    {
        EARANGES_ENSURE(data_);
        return sentinel{*this};
    }
    CPP_member
    auto size() const noexcept -> CPP_ret(std::size_t)(
        requires Sized)
    {
        EARANGES_ENSURE(data_);
        EARANGES_ENSURE(data_->offset_ == -1);
        return static_cast<std::size_t>(data_->n_);
    }
};

#endif
