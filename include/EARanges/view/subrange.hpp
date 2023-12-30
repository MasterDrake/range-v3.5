/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_VIEW_SUBRANGE_HPP
#define EARANGES_VIEW_SUBRANGE_HPP

#include <EASTL/tuple.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/unreachable_sentinel.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/utility/get.hpp>
#include <EARanges/view/interface.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    enum class subrange_kind : bool
    {
        unsized,
        sized
    };

    /// \cond
    namespace detail
    {
        // clang-format off
        /// \concept convertible_to_not_slicing_
        /// \brief The \c convertible_to_not_slicing_ concept
        template<typename From, typename To>
        CPP_concept convertible_to_not_slicing_ =
            convertible_to<From, To> &&
            // A conversion is a slicing conversion if the source and the destination
            // are both pointers, and if the pointed-to types differ after removing
            // cv qualifiers.
            (!(eastl::is_pointer<decay_t<From>>::value &&
                eastl::is_pointer<decay_t<To>>::value &&
                not_same_as_<eastl::remove_pointer_t<decay_t<From>>,
                             eastl::remove_pointer_t<decay_t<To>>>));

        template<std::size_t N, typename T>
        using tuple_element_fun_t = void (*)(meta::_t<eastl::tuple_element<N, T>> const &);

        /// \concept pair_like_impl_
        /// \brief The \c pair_like_impl_ concept
        template<typename T>
        CPP_requires(pair_like_impl_, //
            requires(T t, tuple_element_fun_t<0, T> p0, tuple_element_fun_t<1, T> p1) //
            (
                p0( get<0>(t) ),
                p1( get<1>(t) )
            ));
        /// \concept pair_like_impl_
        /// \brief The \c pair_like_impl_ concept
        template<typename T>
        CPP_concept pair_like_impl_ = CPP_requires_ref(detail::pair_like_impl_, T);

        /// \concept is_complete_
        /// \brief The \c is_complete_ concept
        template(typename T)(
        concept (is_complete_)(T),
            0 != sizeof(T));

        /// \concept is_complete_
        /// \brief The \c is_complete_ concept
        template<typename T>
        CPP_concept is_complete_ = //
            CPP_concept_ref(is_complete_, T);

        template(typename T)( //
        concept (pair_like_)(T), //
            is_complete_<eastl::tuple_size<T>> AND
            derived_from<eastl::tuple_size<T>, meta::size_t<2>> AND
            detail::pair_like_impl_<T>);

        /// \concept pair_like
        /// \brief The \c pair_like concept
        template<typename T>
        CPP_concept pair_like = //
            CPP_concept_ref(detail::pair_like_, T);

        // clang-format off
        template(typename T, typename U, typename V)( //
        concept (pair_like_convertible_from_helper_)(T, U, V), //
            convertible_to_not_slicing_<U, meta::_t<eastl::tuple_element<0, T>>> AND
            convertible_to<V, meta::_t<eastl::tuple_element<1, T>>>);

        /// \concept pair_like_convertible_from_helper_
        /// \brief The \c pair_like_convertible_from_helper_ concept
        template<typename T, typename U, typename V>
        CPP_concept pair_like_convertible_from_helper_ = //
            CPP_concept_ref(pair_like_convertible_from_helper_, T, U, V);

        template(typename T, typename U, typename V)( //
        concept (pair_like_convertible_from_impl_)(T, U, V),
            (!range<T>) AND
            constructible_from<T, U, V> AND
            pair_like<uncvref_t<T>> AND
            pair_like_convertible_from_helper_<T, U, V>);

        /// \concept pair_like_convertible_from_
        /// \brief The \c pair_like_convertible_from_ concept
        template<typename T, typename U, typename V>
        CPP_concept pair_like_convertible_from_ =
            CPP_concept_ref(detail::pair_like_convertible_from_impl_, T, U, V);

        /// \concept range_convertible_to_impl_
        /// \brief The \c range_convertible_to_impl_ concept
        template(typename R, typename I, typename S)(
        concept (range_convertible_to_impl_)(R, I, S),
            convertible_to_not_slicing_<iterator_t<R>, I> AND
            convertible_to<sentinel_t<R>, S>);

        /// \concept range_convertible_to_
        /// \brief The \c range_convertible_to_ concept
        template<typename R, typename I, typename S>
        CPP_concept range_convertible_to_ =
            borrowed_range<R> &&
            CPP_concept_ref(detail::range_convertible_to_impl_, R, I, S);
        // clang-format on

        template(typename S, typename I)(
            requires sentinel_for<S, I>)
        constexpr bool is_sized_sentinel_() noexcept
        {
            return (bool)sized_sentinel_for<S, I>;
        }

        template<subrange_kind K, typename S, typename I>
        constexpr bool store_size_() noexcept
        {
            return K == subrange_kind::sized && !(bool)sized_sentinel_for<S, I>;
        }
    } // namespace detail
    /// \endcond

    template< //
        typename I, //
        typename S = I, //
        subrange_kind K = static_cast<subrange_kind>(detail::is_sized_sentinel_<S, I>())>
    struct subrange;

    template<typename I, typename S, subrange_kind K>
    EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<subrange<I, S, K>> = true;

    /// \cond
    namespace _subrange_
    {
        struct adl_hook
        {};

        template(std::size_t N, typename I, typename S, subrange_kind K)(
            requires (N == 0)) //
        constexpr I get(subrange<I, S, K> const & r)
        {
            return r.begin();
        }
        template(std::size_t N, typename I, typename S, subrange_kind K)(
            requires (N == 1)) //
        constexpr S get(subrange<I, S, K> const & r)
        {
            return r.end();
        }
    } // namespace _subrange_
    /// \endcond

    template<typename I, typename S, subrange_kind K>
    struct subrange
      : view_interface<subrange<I, S, K>,
                       same_as<S, unreachable_sentinel_t>
                           ? infinite
                           : K == subrange_kind::sized ? finite : unknown>
      , private _subrange_::adl_hook
    {
        CPP_assert(input_or_output_iterator<I>);
        CPP_assert(sentinel_for<S, I>);
        CPP_assert(K == subrange_kind::sized || !sized_sentinel_for<S, I>);
        CPP_assert(K != subrange_kind::sized || !same_as<S, unreachable_sentinel_t>);

        using size_type = detail::iter_size_t<I>;
        using iterator = I;
        using sentinel = S;

        subrange() = default;

        template(typename I2)(
            requires detail::convertible_to_not_slicing_<I2, I> AND
            (!detail::store_size_<K, S, I>())) //
        constexpr subrange(I2 && i, S s)
          : data_{static_cast<I2 &&>(i), eastl::move(s)}
        {}

        template(typename I2)(
            requires detail::convertible_to_not_slicing_<I2, I> AND
            (detail::store_size_<K, S, I>())) //
        constexpr subrange(I2 && i, S s, size_type n)
          : data_{static_cast<I2 &&>(i), eastl::move(s), n}
        {
            if(EARANGES_CONSTEXPR_IF((bool)random_access_iterator<I>))
            {
                using D = iter_difference_t<I>;
                EARANGES_EXPECT(n <= (size_type)eastl::numeric_limits<D>::max());
                EARANGES_EXPECT(ranges::next(first_(), (D)n) == last_());
            }
        }
        template(typename I2)(
            requires detail::convertible_to_not_slicing_<I2, I> AND
                sized_sentinel_for<S, I>)
        constexpr subrange(I2 && i, S s, size_type n)
          : data_{static_cast<I2 &&>(i), eastl::move(s)}
        {
            EARANGES_EXPECT(static_cast<size_type>(last_() - first_()) == n);
        }

        template(typename R)(
            requires (!same_as<detail::decay_t<R>, subrange>) AND
                detail::range_convertible_to_<R, I, S> AND
                (!detail::store_size_<K, S, I>()))
        constexpr subrange(R && r)
          : subrange{ranges::begin(r), ranges::end(r)}
        {}

        template(typename R)(
            requires (!same_as<detail::decay_t<R>, subrange>) AND
                detail::range_convertible_to_<R, I, S> AND
                (detail::store_size_<K, S, I>()) AND
                sized_range<R>)
        constexpr subrange(R && r)
          : subrange{ranges::begin(r), ranges::end(r), ranges::size(r)}
        {}

        template(typename R)(
            requires (K == subrange_kind::sized) AND
                detail::range_convertible_to_<R, I, S>)
        constexpr subrange(R && r, size_type n) //
          : subrange{ranges::begin(r), ranges::end(r), n}
        {
            if(EARANGES_CONSTEXPR_IF((bool)sized_range<R>))
            {
                EARANGES_EXPECT(n == ranges::size(r));
            }
        }

        template(typename PairLike)(
            requires (!same_as<PairLike, subrange>) AND
                detail::pair_like_convertible_from_<PairLike, I const &, S const &>)
        constexpr operator PairLike() const
        {
            return PairLike(first_(), last_());
        }

        constexpr I begin() const noexcept(eastl::is_nothrow_copy_constructible<I>::value)
        {
            return first_();
        }
        constexpr S end() const noexcept(eastl::is_nothrow_copy_constructible<S>::value)
        {
            return last_();
        }
        constexpr bool empty() const
        {
            return first_() == last_();
        }

        CPP_member
        constexpr auto size() const //
            -> CPP_ret(size_type)(
                requires (K == subrange_kind::sized))
        {
            return get_size_();
        }

        EARANGES_NODISCARD
        constexpr subrange next(iter_difference_t<I> n = 1) const
        {
            auto tmp = *this;
            tmp.advance(n);
            return tmp;
        }

        CPP_member
        EARANGES_NODISCARD constexpr auto prev(iter_difference_t<I> n = 1) const
            -> CPP_ret(subrange)(
                requires bidirectional_iterator<I>)
        {
            auto tmp = *this;
            tmp.advance(-n);
            return tmp;
        }

        constexpr subrange & advance(iter_difference_t<I> n)
        {
            set_size_(static_cast<size_type>(n - ranges::advance(first_(), n, last_())));
            return *this;
        }

    private:
        using data_t =
            meta::conditional_t< //
                detail::store_size_<K, S, I>(), //
            eastl::tuple<I, S, size_type>,  //
            eastl::tuple<I, S>>;
        data_t data_;

        constexpr I & first_() noexcept
        {
            return eastl::get<0>(data_);
        }
        constexpr const I & first_() const noexcept
        {
            return eastl::get<0>(data_);
        }
        constexpr S & last_() noexcept
        {
            return eastl::get<1>(data_);
        }
        constexpr const S & last_() const noexcept
        {
            return eastl::get<1>(data_);
        }
        CPP_member
        constexpr auto get_size_() const //
            -> CPP_ret(size_type)(
                requires sized_sentinel_for<S, I>)
        {
            return static_cast<size_type>(last_() - first_());
        }
        CPP_member
        constexpr auto get_size_() const noexcept //
            -> CPP_ret(size_type)(
                requires (detail::store_size_<K, S, I>()))
        {
            return eastl::get<2>(data_);
        }
        static constexpr void set_size_(...) noexcept
        {}
        CPP_member
        constexpr auto set_size_(size_type n) noexcept //
            -> CPP_ret(void)(
                requires (detail::store_size_<K, S, I>()))
        {
            eastl::get<2>(data_) = get_size_() - n;
        }
    };

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
    template<typename I, typename S>
    subrange(I, S) //
        -> subrange<I, S>;

    template(typename I, typename S)(
        requires input_or_output_iterator<I> AND sentinel_for<S, I>)
    subrange(I, S, detail::iter_size_t<I>)
        -> subrange<I, S, subrange_kind::sized>;

    template(typename R)(
        requires borrowed_range<R>)
    subrange(R &&) //
        -> subrange<iterator_t<R>, sentinel_t<R>,
                    (sized_range<R> ||
                        sized_sentinel_for<sentinel_t<R>, iterator_t<R>>)
                           ? subrange_kind::sized
                           : subrange_kind::unsized>;

    template(typename R)(
        requires borrowed_range<R>)
    subrange(R &&, detail::iter_size_t<iterator_t<R>>)
        -> subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;
#endif

    // in lieu of deduction guides, use make_subrange
    struct make_subrange_fn
    {
        template<typename I, typename S>
        constexpr subrange<I, S> operator()(I i, S s) const
        {
            return {i, s};
        }
        template(typename I, typename S)(requires input_or_output_iterator<I> AND sentinel_for<S, I>)
        constexpr subrange<I, S, subrange_kind::sized> //
        operator()(I i, S s, detail::iter_size_t<I> n) const
        {
            return {i, s, n};
        }
        template(typename R)(requires borrowed_range<R>)
        constexpr auto operator()(R && r) const
            -> subrange<iterator_t<R>, sentinel_t<R>,
                     (sized_range<R> || sized_sentinel_for<sentinel_t<R>, iterator_t<R>>)
                         ? subrange_kind::sized
                         : subrange_kind::unsized>
        {
            return {(R &&) r};
        }
        template(typename R)(requires borrowed_range<R>)
        constexpr subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized> //
        operator()(R && r, detail::iter_size_t<iterator_t<R>> n) const
        {
            return {(R &&) r, n};
        }
    };

    /// \relates make_subrange_fn
    /// \ingroup group-views
    EARANGES_INLINE_VARIABLE(make_subrange_fn, make_subrange)

    template<typename R>
    using borrowed_subrange_t = detail::maybe_dangling_<R, subrange<iterator_t<R>>>;

    /// @}
} // namespace ranges

EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

//namespace std
//{
//    template<typename I, typename S, ::ranges::subrange_kind K>
//    struct tuple_size<::ranges::subrange<I, S, K>> : eastl::integral_constant<size_t, 2>
//    {};
//    template<typename I, typename S, ::ranges::subrange_kind K>
//    struct tuple_element<0, ::ranges::subrange<I, S, K>>
//    {
//        using type = I;
//    };
//    template<typename I, typename S, ::ranges::subrange_kind K>
//    struct tuple_element<1, ::ranges::subrange<I, S, K>>
//    {
//        using type = S;
//    };
//} // namespace std

namespace eastl
{
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_size<::ranges::subrange<I, S, K>> : eastl::integral_constant<size_t, 2>
    {};
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_element<0, ::ranges::subrange<I, S, K>>
    {
        using type = I;
    };
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_element<1, ::ranges::subrange<I, S, K>>
    {
        using type = S;
    };
} // namespace eastl
EARANGES_DIAGNOSTIC_POP

#include <EARanges/detail/epilogue.hpp>

#endif