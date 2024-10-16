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

#ifndef EARANGES_RANGE_PRIMITIVES_HPP
#define EARANGES_RANGE_PRIMITIVES_HPP

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/utility/addressof.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

#include <EASTL/string.h>

namespace ranges
{
    /// \addtogroup group-range
    // Specialize this if the default is wrong.
    template<typename T>
    EARANGES_INLINE_VAR constexpr bool disable_sized_range = false;

    /// \cond
    namespace _size_
    {
        template<typename T>
        void size(T &&) = delete;

#ifdef EARANGES_WORKAROUND_MSVC_895622
        void size();
#endif

        // clang-format off
        /// \concept has_member_size_
        /// \brief The \c has_member_size_ concept
        template<typename T>
        CPP_requires(has_member_size_,
            requires(T && t) //
            (
                ((T &&) t).size()
            ));
        /// \concept has_member_size
        /// \brief The \c has_member_size concept
        template<typename T>
        CPP_concept has_member_size =
            CPP_requires_ref(_size_::has_member_size_, T);

        /// \concept has_non_member_size_
        /// \brief The \c has_non_member_size_ concept
        template<typename T>
        CPP_requires(has_non_member_size_,
            requires(T && t) //
            (
                size((T &&) t)
            ));
        /// \concept has_non_member_size
        /// \brief The \c has_non_member_size concept
        template<typename T>
        CPP_concept has_non_member_size =
            CPP_requires_ref(_size_::has_non_member_size_, T);
        // clang-format on

        struct fn
        {
        private:
            struct _member_result_
            {
                template<typename R>
                using invoke = decltype(+(declval(R &).size()));
            };
            struct _non_member_result_
            {
                template<typename R>
                using invoke = decltype(+(size(declval(R &))));
            };
            struct _distance_result_
            {
                template<typename R>
                using invoke = detail::iter_size_t<_begin_::_t<R>>;
            };
            struct _other_result_
            {
                template<typename R>
                using invoke =
                    meta::invoke<
                        meta::conditional_t<
                            has_non_member_size<R>,
                            _non_member_result_,
                            _distance_result_>,
                        R>;
            };

            template<typename R>
            using _result_t =
                meta::invoke<
                    meta::conditional_t<
                        has_member_size<R>,
                        _member_result_,
                        _other_result_>,
                    R>;

        public:
            template<typename R, eastl::size_t N>
            constexpr eastl::size_t operator()(R (&)[N], int) const noexcept
            {
                return N;
            }

            template<typename R, eastl::size_t N>
            constexpr eastl::size_t operator()(R(&&)[N]) const noexcept
            {
                return N;
            }

            // Prefer member if it returns integral.
            template(typename R)(
                requires (!disable_sized_range<uncvref_t<R>>) AND
                    has_member_size<R> AND detail::integer_like_<_result_t<R>>)
            constexpr _result_t<R> operator()(R && r) const
                noexcept(noexcept(((R &&) r).size()))
            {
                return ((R &&) r).size();
            }

            // Use ADL if it returns integral.
            template(typename R)(
                requires (!disable_sized_range<uncvref_t<R>>) AND
                    (!has_member_size<R>) AND has_non_member_size<R> AND
                    detail::integer_like_<_result_t<R>>)
            constexpr _result_t<R> operator()(R && r) const
                noexcept(noexcept(size((R &&) r)))
            {
                return size((R &&) r);
            }

            template(typename R)(
                requires (!has_member_size<R> || disable_sized_range<uncvref_t<R>>) AND
                    (!has_non_member_size<R> || disable_sized_range<uncvref_t<R>>) AND
                    forward_iterator<_begin_::_t<R>> AND
                    sized_sentinel_for<_end_::_t<R>, _begin_::_t<R>>)
            constexpr _result_t<R> operator()(R && r) const
                noexcept(noexcept(ranges::end((R &&) r) - ranges::begin((R &&) r)))
            {
                using size_type = detail::iter_size_t<_begin_::_t<R>>;
                return static_cast<size_type>(ranges::end((R &&) r) -
                                              ranges::begin((R &&) r));
            }
        };
    } // namespace _size_
    /// \endcond

    /// \ingroup group-range
    /// \return For a given expression `E` of type `T`, `ranges::size(E)` is equivalent
    /// to:
    ///   * `+extent_v<T>` if `T` is an array type.
    ///   * Otherwise, `+E.size()` if it is a valid expression and its type `I` models
    ///     `integral` and `disable_sized_range<eastl::remove_cvref_t<T>>` is false.
    ///   * Otherwise, `+size(E)` if it is a valid expression and its type `I` models
    ///     `integral` with overload resolution performed in a context that includes the
    ///     declaration:
    ///     \code
    ///     template<class T> void size(T&&) = delete;
    ///     \endcode
    ///     and does not include a declaration of `ranges::size`, and
    ///     `disable_sized_range<eastl::remove_cvref_t<T>>` is false.
    ///   * Otherwise, `static_cast<U>(ranges::end(E) - ranges::begin(E))` where `U` is
    ///     `eastl::make_unsigned_t<iter_difference_t<iterator_t<T>>>` if
    ///     `iter_difference_t<iterator_t<T>>` satisfies `integral` and
    ///     `iter_difference_t<iterator_t<T>>` otherwise; except that `E` is
    ///     evaluated once, if it is a valid expression and the types `I` and `S` of
    ///     `ranges::begin(E)` and `ranges::end(E)` model `sized_sentinel_for<S, I>` and
    ///     `forward_iterator<I>`.
    ///   * Otherwise, `ranges::size(E)` is ill-formed.
    EARANGES_DEFINE_CPO(_size_::fn, size)

    // Customization point data
    /// \cond
    namespace _data_
    {
        // clang-format off
        /// \concept has_member_data_
        /// \brief The \c has_member_data_ concept
        template<typename T>
        CPP_requires(has_member_data_,
            requires(T & t) //
            (
                t.data()
            ));
        /// \concept has_member_data
        /// \brief The \c has_member_data concept
        template<typename T>
        CPP_concept has_member_data =
            CPP_requires_ref(_data_::has_member_data_, T);
        // clang-format on

        struct fn
        {
        private:
            struct _member_data_
            {
                template<typename R>
                using invoke = decltype(+(declval(R &&).data()));
            };
            struct _pointer_iterator_
            {
                template<typename R>
                using invoke = _begin_::_t<R>;
            };
            struct _contiguous_iterator_
            {
                template<typename R>
                using invoke = decltype(detail::addressof(*declval(_begin_::_t<R> &&)));
            };
            struct _other_result_
            {
                template<typename R>
                using invoke =
                    meta::invoke<
                        meta::conditional_t<
                            eastl::is_pointer<_begin_::_t<R>>::value,
                            _pointer_iterator_,
                            _contiguous_iterator_>,
                        R>;
            };

            template<typename R>
            using _result_t =
                meta::invoke<
                    meta::conditional_t<
                        has_member_data<R>,
                        _member_data_,
                        _other_result_>,
                    R>;

        public:
            template(typename R)(
                requires has_member_data<R &> AND
                    eastl::is_pointer<_result_t<R &>>::value)
            constexpr _result_t<R &> operator()(R & r) const //
                noexcept(noexcept(r.data()))
            {
                return r.data();
            }
            template(typename R)(
                requires (!has_member_data<R &>) AND
                    eastl::is_pointer<_begin_::_t<R>>::value)
            constexpr _result_t<R> operator()(R && r) const //
                noexcept(noexcept(ranges::begin((R &&) r)))
            {
                return ranges::begin((R &&) r);
            }
            template(typename R)(
                requires (!has_member_data<R &>) AND
                    (!eastl::is_pointer<_begin_::_t<R>>::value) AND
                    contiguous_iterator<_begin_::_t<R>>)
            constexpr _result_t<R> operator()(R && r) const //
                noexcept(noexcept(
                    ranges::begin((R &&) r) == ranges::end((R &&) r)
                      ? nullptr
                      : detail::addressof(*ranges::begin((R &&) r))))
            {
                return ranges::begin((R &&) r) == ranges::end((R &&) r)
                  ? nullptr
                  : detail::addressof(*ranges::begin((R &&) r));
            }
        };

        template<typename R>
        using _t = decltype(fn{}(declval(R &&)));
    } // namespace _data_
    /// \endcond

    EARANGES_INLINE_VARIABLE(_data_::fn, data)

    /// \cond
    namespace _cdata_
    {
        struct fn
        {
            template<typename R>
            constexpr _data_::_t<R const &> operator()(R const & r) const
                noexcept(noexcept(ranges::data(r)))
            {
                return ranges::data(r);
            }
            template<typename R>
            constexpr _data_::_t<R const> operator()(R const && r) const
                noexcept(noexcept(ranges::data((R const &&)r)))
            {
                return ranges::data((R const &&)r);
            }
        };
    } // namespace _cdata_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return The result of calling `ranges::data` with a const-qualified
    ///    (lvalue or rvalue) reference to `r`.
    EARANGES_INLINE_VARIABLE(_cdata_::fn, cdata)

    /// \cond
    namespace _empty_
    {
        // clang-format off
        /// \concept has_member_empty_
        /// \brief The \c has_member_empty_ concept
        template<typename T>
        CPP_requires(has_member_empty_,
            requires(T && t) //
            (
                bool(((T &&) t).empty())
            ));
        /// \concept has_member_empty
        /// \brief The \c has_member_empty concept
        template<typename T>
        CPP_concept has_member_empty =
            CPP_requires_ref(_empty_::has_member_empty_, T);

        /// \concept has_size_
        /// \brief The \c has_size_ concept
        template<typename T>
        CPP_requires(has_size_,
            requires(T && t) //
            (
                ranges::size((T &&) t)
            ));
        /// \concept has_size
        /// \brief The \c has_size concept
        template<typename T>
        CPP_concept has_size =
            CPP_requires_ref(_empty_::has_size_, T);
        // clang-format on

        struct fn
        {
            // Prefer member if it is valid.
            template(typename R)(requires has_member_empty<R>)
            constexpr bool operator()(R && r) const noexcept(noexcept(bool(((R &&) r).empty())))
            {
                return bool(((R &&) r).empty());
            }

            // Fall back to size == 0.
            template(typename R)(requires (!has_member_empty<R>) AND has_size<R>)
            constexpr bool operator()(R && r) const noexcept(noexcept(bool(ranges::size((R &&) r) == 0)))
            {
                return bool(ranges::size((R &&) r) == 0);
            }

            // Fall further back to begin == end.
            template(typename R)(requires (!has_member_empty<R>) AND (!has_size<R>) AND forward_iterator<_begin_::_t<R>>)
            constexpr bool operator()(R && r) const noexcept(noexcept(bool(ranges::begin((R &&) r) == ranges::end((R &&) r))))
            {
                return bool(ranges::begin((R &&) r) == ranges::end((R &&) r));
            }
        };
    } // namespace _empty_
    /// \endcond

    /// \ingroup group-range
    /// \return true if and only if range contains no elements.
    EARANGES_INLINE_VARIABLE(_empty_::fn, empty)
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif
