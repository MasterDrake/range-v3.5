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
#ifndef RANGES_V3_ITERATOR_INSERT_ITERATORS_HPP
#define RANGES_V3_ITERATOR_INSERT_ITERATORS_HPP

#include <cstddef>
#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../iterator/operations.hpp"
#include "../utility/addressof.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{
    template<typename Container>
    struct back_insert_iterator
    {
        using container_type = Container;
        using difference_type = std::ptrdiff_t;

        constexpr back_insert_iterator() = default;
        constexpr explicit back_insert_iterator(Container & x) : container_(detail::addressof(x))
        {}
        back_insert_iterator & operator=(typename Container::value_type const & value)
        {
            container_->push_back(value);
            return *this;
        }
        back_insert_iterator & operator=(typename Container::value_type && value)
        {
            container_->push_back(eastl::move(value));
            return *this;
        }
        back_insert_iterator & operator*()
        {
            return *this;
        }
        back_insert_iterator & operator++()
        {
            return *this;
        }
        back_insert_iterator operator++(int)
        {
            return *this;
        }

    private:
        Container * container_ = nullptr;
    };

    struct back_inserter_fn
    {
        template<typename Container>
        constexpr back_insert_iterator<Container> operator()(Container & x) const
        {
            return back_insert_iterator<Container>{x};
        }
    };

    /// \sa `back_inserter_fn`
    RANGES_INLINE_VARIABLE(back_inserter_fn, back_inserter)

    template<typename Container>
    struct front_insert_iterator
    {
        using container_type = Container;
        using difference_type = std::ptrdiff_t;

        constexpr front_insert_iterator() = default;
        constexpr explicit front_insert_iterator(Container & x) : container_(detail::addressof(x))
        {}
        front_insert_iterator & operator=(typename Container::value_type const & value)
        {
            container_->push_front(value);
            return *this;
        }
        front_insert_iterator & operator=(typename Container::value_type && value)
        {
            container_->push_front(eastl::move(value));
            return *this;
        }
        front_insert_iterator & operator*()
        {
            return *this;
        }
        front_insert_iterator & operator++()
        {
            return *this;
        }
        front_insert_iterator operator++(int)
        {
            return *this;
        }

    private:
        Container * container_ = nullptr;
    };

    struct front_inserter_fn
    {
        template<typename Cont>
        constexpr front_insert_iterator<Cont> operator()(Cont & cont) const
        {
            return front_insert_iterator<Cont>{cont};
        }
    };

    /// \sa `front_inserter_fn`
    RANGES_INLINE_VARIABLE(front_inserter_fn, front_inserter)

    template<typename Container>
    struct insert_iterator
    {
        using container_type = Container;
        using difference_type = std::ptrdiff_t;

        constexpr insert_iterator() = default;
        constexpr explicit insert_iterator(Container & x, typename Container::iterator w)
          : container_(detail::addressof(x))
          , where_(w)
        {}
        insert_iterator & operator=(typename Container::value_type const & value)
        {
            where_ = ranges::next(container_->insert(where_, value));
            return *this;
        }
        insert_iterator & operator=(typename Container::value_type && value)
        {
            where_ = ranges::next(container_->insert(where_, eastl::move(value)));
            return *this;
        }
        insert_iterator & operator*()
        {
            return *this;
        }
        insert_iterator & operator++()
        {
            return *this;
        }
        insert_iterator & operator++(int)
        {
            return *this;
        }

    private:
        Container * container_ = nullptr;
        typename Container::iterator where_ = typename Container::iterator();
    };

    struct inserter_fn
    {
        template<typename Cont>
        constexpr insert_iterator<Cont> operator()(Cont & cont, typename Cont::iterator where) const
        {
            return insert_iterator<Cont>{cont, eastl::move(where)};
        }
    };

    /// \sa `inserter_fn`
    RANGES_INLINE_VARIABLE(inserter_fn, inserter)

    /// @}
} // namespace ranges

/// \cond
RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

//namespace std
//{
//    template<typename Container>
//    struct iterator_traits<::ranges::back_insert_iterator<Container>>
//      : ::ranges::detail::std_output_iterator_traits<>
//    {};
//
//    template<typename Container>
//    struct iterator_traits<::ranges::front_insert_iterator<Container>>
//      : ::ranges::detail::std_output_iterator_traits<>
//    {};
//
//    template<typename Container>
//    struct iterator_traits<::ranges::insert_iterator<Container>>
//      : ::ranges::detail::std_output_iterator_traits<>
//    {};
//} // namespace std

namespace eastl
{
    template<typename Container>
    struct iterator_traits<::ranges::back_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Container>
    struct iterator_traits<::ranges::front_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Container>
    struct iterator_traits<::ranges::insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};
} // namespace std

RANGES_DIAGNOSTIC_POP
/// \endcond

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_ITERATOR_INSERT_ITERATORS_HPP