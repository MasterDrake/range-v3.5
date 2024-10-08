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
#ifndef EARANGES_ITERATOR_COUNTED_ITERATOR_HPP
#define EARANGES_ITERATOR_COUNTED_ITERATOR_HPP

#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    /// \cond
    namespace _counted_iterator_
    {
        struct access
        {
            template<typename I>
            static constexpr iter_difference_t<counted_iterator<I>> & count(counted_iterator<I> & ci) noexcept
            {
                return ci.cnt_;
            }

            template<typename I>
            static constexpr I & current(counted_iterator<I> & ci) noexcept
            {
                return ci.current_;
            }

            template<typename I>
            static constexpr I const & current(counted_iterator<I> const & ci) noexcept
            {
                return ci.current_;
            }
        };

        template<bool>
        struct contiguous_iterator_concept_base
        {};

        template<>
        struct contiguous_iterator_concept_base<true>
        {
            using iterator_concept = ranges::contiguous_iterator_tag;
        };
    } // namespace _counted_iterator_
    /// \endcond

    template<typename I>
        // requires input_or_output_iterator<I>
    struct counted_iterator : _counted_iterator_::contiguous_iterator_concept_base<(bool)contiguous_iterator<I>>
    {
    private:
        friend advance_fn;
        CPP_assert(input_or_output_iterator<I>);
        friend _counted_iterator_::access;

        I current_{};
        iter_difference_t<I> cnt_{0};

        constexpr void post_increment_(eastl::true_type)
        {
            CPP_assert(eastl::is_void<decltype(current_++)>());
            ++current_;
            --cnt_;
        }
        constexpr auto post_increment_(eastl::false_type) -> decltype(current_++)
        {
            CPP_assert(!eastl::is_void<decltype(current_++)>());
            auto && tmp = current_++;
            --cnt_;
            return static_cast<decltype(tmp) &&>(tmp);
        }

    public:
        using iterator_type = I;
        using difference_type = iter_difference_t<I>;

        counted_iterator() = default;

        constexpr counted_iterator(I x, iter_difference_t<I> n)
          : current_(eastl::move(x))
          , cnt_(n)
        {
            EARANGES_EXPECT(n >= 0);
        }

        template(typename I2)(requires convertible_to<I2, I>)
        constexpr counted_iterator(counted_iterator<I2> const & i)
          : current_(_counted_iterator_::access::current(i))
          , cnt_(i.count())
        {}

        template(typename I2)(requires convertible_to<I2, I>)
        constexpr counted_iterator & operator=(counted_iterator<I2> const & i)
        {
            current_ = _counted_iterator_::access::current(i);
            cnt_ = i.count();
        }

        constexpr I base() const
        {
            return current_;
        }

        constexpr iter_difference_t<I> count() const
        {
            return cnt_;
        }

        constexpr iter_reference_t<I> operator*() noexcept(noexcept(iter_reference_t<I>(*current_)))
        {
            EARANGES_EXPECT(cnt_ > 0);
            return *current_;
        }
        template(typename I2 = I)(requires indirectly_readable<I2 const>)
        constexpr iter_reference_t<I2> operator*() const //
            noexcept(noexcept(iter_reference_t<I>(*current_)))
        {
            EARANGES_EXPECT(cnt_ > 0);
            return *current_;
        }

        constexpr counted_iterator & operator++()
        {
            EARANGES_EXPECT(cnt_ > 0);
            ++current_;
            --cnt_;
            return *this;
        }

#ifdef EARANGES_WORKAROUND_MSVC_677925
        template(typename I2 = I)(requires (!forward_iterator<I2>)) //
        constexpr auto operator++(int) -> decltype(eastl::declval<I2 &>()++)
#else  // ^^^ workaround ^^^ / vvv no workaround vvv
        CPP_member
        constexpr auto operator++(int) //
            -> CPP_ret(decltype(eastl::declval<I &>()++))(
                requires (!forward_iterator<I>))
#endif // EARANGES_WORKAROUND_MSVC_677925
        {
            EARANGES_EXPECT(cnt_ > 0);
            return post_increment_(eastl::is_void<decltype(current_++)>());
        }

        CPP_member
        constexpr auto operator++(int) //
            -> CPP_ret(counted_iterator)(requires forward_iterator<I>)
        {
            auto tmp(*this);
            ++*this;
            return tmp;
        }

        CPP_member
        constexpr auto operator--() //
            -> CPP_ret(counted_iterator &)(requires bidirectional_iterator<I>)
        {
            --current_;
            ++cnt_;
            return *this;
        }

        CPP_member
        constexpr auto operator--(int) //
            -> CPP_ret(counted_iterator)(requires bidirectional_iterator<I>)
        {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        CPP_member
        constexpr auto operator+=(difference_type n) //
            -> CPP_ret(counted_iterator &)(requires random_access_iterator<I>)
        {
            EARANGES_EXPECT(cnt_ >= n);
            current_ += n;
            cnt_ -= n;
            return *this;
        }

        CPP_member
        constexpr auto operator+(difference_type n) const //
            -> CPP_ret(counted_iterator)(requires random_access_iterator<I>)
        {
            auto tmp(*this);
            tmp += n;
            return tmp;
        }

        CPP_member
        constexpr auto operator-=(difference_type n) //
            -> CPP_ret(counted_iterator &)(requires random_access_iterator<I>)
        {
            EARANGES_EXPECT(cnt_ >= -n);
            current_ -= n;
            cnt_ += n;
            return *this;
        }

        CPP_member
        constexpr auto operator-(difference_type n) const //
            -> CPP_ret(counted_iterator)(requires random_access_iterator<I>)
        {
            auto tmp(*this);
            tmp -= n;
            return tmp;
        }

        CPP_member
        constexpr auto operator[](difference_type n) const //
            -> CPP_ret(iter_reference_t<I>)(requires random_access_iterator<I>)
        {
            EARANGES_EXPECT(cnt_ >= n);
            return current_[n];
        }

#if !EARANGES_BROKEN_CPO_LOOKUP
        CPP_broken_friend_member
        friend constexpr auto iter_move(counted_iterator const & i) //
            noexcept(detail::has_nothrow_iter_move_v<I>)
            -> CPP_broken_friend_ret(iter_rvalue_reference_t<I>)(requires input_iterator<I>)
        {
            return ranges::iter_move(i.current_);
        }
        template<typename I2, typename S2>
        friend constexpr auto iter_swap(counted_iterator const & x,
                                        counted_iterator<I2> const & y) //
            noexcept(is_nothrow_indirectly_swappable<I, I2>::value)
            -> CPP_broken_friend_ret(void)(requires indirectly_swappable<I2, I>)
        {
            return ranges::iter_swap(x.current_, _counted_iterator_::access::current(y));
        }
#endif
    };

    /// \cond
#if EARANGES_BROKEN_CPO_LOOKUP
    namespace _counted_iterator_
    {
        template<typename I>
        constexpr auto iter_move(counted_iterator<I> const & i) noexcept(
            detail::has_nothrow_iter_move_v<I>)
            -> CPP_broken_friend_ret(iter_rvalue_reference_t<I>)(
                requires input_iterator<I>)
        {
            return ranges::iter_move(_counted_iterator_::access::current(i));
        }
        template<typename I1, typename I2>
        constexpr auto iter_swap(
            counted_iterator<I1> const & x,
            counted_iterator<I2> const &
                y) noexcept(is_nothrow_indirectly_swappable<I1, I2>::value)
            -> CPP_broken_friend_ret(void)(
                requires indirectly_swappable<I2, I1>)
        {
            return ranges::iter_swap(_counted_iterator_::access::current(x),
                                     _counted_iterator_::access::current(y));
        }
    } // namespace _counted_iterator_
#endif
    /// endcond

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr bool operator==(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return x.count() == y.count();
    }

    template<typename I>
    constexpr bool operator==(counted_iterator<I> const & x, default_sentinel_t)
    {
        return x.count() == 0;
    }

    template<typename I>
    constexpr bool operator==(default_sentinel_t, counted_iterator<I> const & x)
    {
        return x.count() == 0;
    }

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr bool operator!=(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return !(x == y);
    }

    template<typename I>
    constexpr bool operator!=(counted_iterator<I> const & x, default_sentinel_t y)
    {
        return !(x == y);
    }

    template<typename I>
    constexpr bool operator!=(default_sentinel_t x, counted_iterator<I> const & y)
    {
        return !(x == y);
    }

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr bool operator<(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return y.count() < x.count();
    }

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr bool operator<=(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return !(y < x);
    }

    template(typename I1, typename I2)( requires common_with<I1, I2>)
    constexpr bool operator>(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return y < x;
    }

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr bool operator>=(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return !(x < y);
    }

    template(typename I1, typename I2)(requires common_with<I1, I2>)
    constexpr iter_difference_t<I2> operator-(counted_iterator<I1> const & x, counted_iterator<I2> const & y)
    {
        return y.count() - x.count();
    }

    template<typename I>
    constexpr iter_difference_t<I> operator-(counted_iterator<I> const & x, default_sentinel_t)
    {
        return -x.count();
    }

    template<typename I>
    constexpr iter_difference_t<I> operator-(default_sentinel_t, counted_iterator<I> const & y)
    {
        return y.count();
    }

    template(typename I)(requires random_access_iterator<I>)
    constexpr counted_iterator<I> operator+(iter_difference_t<I> n,  counted_iterator<I> const & x)
    {
        return x + n;
    }

    template(typename I)(requires input_or_output_iterator<I>)
    constexpr counted_iterator<I> make_counted_iterator(I i, iter_difference_t<I> n)
    {
        return {eastl::move(i), n};
    }

    template<typename I>
    struct indirectly_readable_traits<counted_iterator<I>>
      : meta::conditional_t<
            (bool)indirectly_readable<I>,
            indirectly_readable_traits<I>,
            meta::nil_>
    {};

    CPP_template_def(typename I)(requires input_or_output_iterator<I>)
    constexpr void advance_fn::operator()(counted_iterator<I> & i, iter_difference_t<I> n) const
    {
        EARANGES_EXPECT(n <= i.cnt_);
        advance(i.current_, n);
        i.cnt_ -= n;
    }
    /// @}
} // namespace ranges

/// \cond
namespace ranges
{
    namespace _counted_iterator_
    {
        template<typename I, typename = void>
        struct iterator_traits_
        {
            using difference_type = iter_difference_t<I>;
            using value_type = void;
            using reference = void;
            using pointer = void;
            using iterator_category = eastl::output_iterator_tag;
        };

        template<typename I>
        struct iterator_traits_<I, meta::if_c<input_iterator<I>>> : eastl::iterator_traits<I>
        {
            using pointer = void;
        };
    } // namespace _counted_iterator_
} // namespace ranges

namespace eastl
{
    template<typename I>
    struct iterator_traits<::ranges::counted_iterator<I>> : ::ranges::_counted_iterator_::iterator_traits_<I>
    {};
} // namespace eastl
/// \endcond

#include <EARanges/detail/epilogue.hpp>

#endif
