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

#ifndef EARANGES_UTILITY_VARIANT_HPP
#define EARANGES_UTILITY_VARIANT_HPP

#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/detail/variant.hpp>
#include <EARanges/iterator/move_iterators.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/primitives.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T, eastl::size_t N, typename Index>
        struct indexed_datum<T[N], Index>
        {
        private:
            union
            {
                char c;
                T data_[N];
            };
            void fill_default_(T * p, eastl::true_type)
            {
                for(; p != ranges::end(data_); ++p)
                    ::new((void *)p) T{};
            }
            void fill_default_(T * p, eastl::false_type)
            {
                EARANGES_EXPECT(p == ranges::end(data_));
            }

        public:
            CPP_member
            constexpr CPP_ctor(indexed_datum)(meta::nil_ = {})(
                requires default_constructible<T>)
              : data_{}
            {}
            CPP_member
            CPP_ctor(indexed_datum)(indexed_datum && that)(
                requires move_constructible<T>)
            {
                eastl::uninitialized_copy_n(make_move_iterator(that.data_), N, data_);
            }
            CPP_member
            CPP_ctor(indexed_datum)(indexed_datum const & that)(requires copy_constructible<T>)
            {
                eastl::uninitialized_copy_n(that.data_, N, data_);
            }
            // \pre Requires distance(first, last) <= N
            // \pre Requires default_constructible<T> || distance(first, last) == N
            template(typename I, typename S)(
                requires sentinel_for<S, I> AND input_iterator<I> AND
                    constructible_from<T, iter_reference_t<I>>)
            indexed_datum(I first, S last)
            {
                T * p = detail::uninitialized_copy(first, last, data_);
                this->fill_default_(p, meta::bool_<default_constructible<T>>{});
            }
            // \pre Requires distance(r) <= N
            // \pre Requires default_constructible<T> || distance(r) == N
            template(typename R)(requires input_range<R> AND constructible_from<T, range_reference_t<R>>)
            explicit indexed_datum(R && r)
              : indexed_datum{ranges::begin(r), ranges::end(r)}
            {}
            CPP_member
            auto operator=(indexed_datum && that) //
                -> CPP_ret(indexed_datum &)(requires assignable_from<T &, T>)
            {
                ranges::move(that.data_, data_);
                return *this;
            }
            CPP_member
            auto operator=(indexed_datum const & that) //
                -> CPP_ret(indexed_datum &)(requires assignable_from<T &, T const &>)
            {
                ranges::copy(that.data_, data_);
                return *this;
            }
            // \pre Requires ranges::distance(r) <= N
            template(typename R)(requires input_range<R> AND assignable_from<T &, range_reference_t<R>>)
            indexed_datum & operator=(R && r)
            {
                ranges::copy(r, data_);
                return *this;
            }
            constexpr auto ref()
            {
                return indexed_element<T(&)[N], Index::value>{data_};
            }
            constexpr auto ref() const
            {
                return indexed_element<T const(&)[N], Index::value>{data_};
            }
            constexpr T (&get() noexcept)[N]
            {
                return data_;
            }
            constexpr T const (&get() const noexcept)[N]
            {
                return data_;
            }
        };
    } // namespace detail
    /// \endcond
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
