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

#ifndef EARANGES_VIEW_IOTA_HPP
#define EARANGES_VIEW_IOTA_HPP

#include <climits>
#include <cstdint>
#include <EASTL/numeric_limits.h>
//#include <EASTL/type_traits.h>
//#include <EASTL/utility.h>

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/iterator/default_sentinel.hpp>
#include <EARanges/iterator/diffmax_t.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/delimit.hpp>
#include <EARanges/view/facade.hpp>

#include <EARanges/detail/prologue.hpp>

EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_UNSIGNED_MATH
EARANGES_DIAGNOSTIC_IGNORE_TRUNCATION

namespace eastl
{
    namespace ranges
    {
        /// \cond
        namespace detail
        {
            template<std::size_t N, typename = void>
            struct promote_as_signed_
            {
                // This shouldn't cause us to LOSE precision, but maybe it doesn't net us
                // any either.
                static_assert(sizeof(std::intmax_t) * CHAR_BIT >= N,
                              "Possible extended integral type?");
                using difference_type = diffmax_t;
            };

            template<std::size_t N>
            struct promote_as_signed_<N, enable_if_t<(N < 16)>>
            {
                using difference_type = std::int_fast16_t;
            };

            template<std::size_t N>
            struct promote_as_signed_<N, enable_if_t<(N >= 16 && N < 32)>>
            {
                using difference_type = std::int_fast32_t;
            };

            template<std::size_t N>
            struct promote_as_signed_<N, enable_if_t<(N >= 32 && N < 64)>>
            {
                using difference_type = std::int_fast64_t;
            };

            template<typename I>
            using iota_difference_t = typename meta::conditional_t<
                eastl::is_integral<I>::value && sizeof(I) == sizeof(iter_difference_t<I>),
                promote_as_signed_<sizeof(iter_difference_t<I>) * CHAR_BIT>,
                with_difference_type_<iter_difference_t<I>>>::difference_type;

            // clang-format off
        /// \concept _decrementable_
        /// \brief The \c _decrementable_ concept
        template<typename I>
        CPP_requires(_decrementable_,
            requires(I i) //
            (
                --i,
                i--,
                concepts::requires_<same_as<I&, decltype(--i)>>,
                concepts::requires_<same_as<I, decltype(i--)>>
            ));
        /// \concept decrementable_
        /// \brief The \c decrementable_ concept
        template<typename I>
        CPP_concept decrementable_ =
            incrementable<I> &&
            CPP_requires_ref(detail::_decrementable_, I);

        /// \concept _advanceable_
        /// \brief The \c _advanceable_ concept
        template<typename I>
        CPP_requires(_advanceable_,
            requires(I i, I const j, iota_difference_t<I> const n) //
            (
                j - j,
                i += n,
                i -= n,
                static_cast<I>(j - n),
                static_cast<I>(j + n),
                static_cast<I>(n + j),
                // NOT TO SPEC:
                // Unsigned integers are advanceable, but subtracting them results in
                // an unsigned integral, which is not the same as the difference type,
                // which is signed.
                concepts::requires_<convertible_to<decltype(j - j), iota_difference_t<I>>>,
                concepts::requires_<same_as<I&, decltype(i += n)>>,
                concepts::requires_<same_as<I&, decltype(i -= n)>> //,
                // concepts::requires_<convertible_to<decltype(i - n), I>>,
                // concepts::requires_<convertible_to<decltype(i + n), I>>,
                // concepts::requires_<convertible_to<decltype(n + i), I>>
            ));
        /// \concept advanceable_
        /// \brief The \c advanceable_ concept
        template<typename I>
        CPP_concept advanceable_ =
            decrementable_<I> && totally_ordered<I> &&
            CPP_requires_ref(detail::_advanceable_, I);
            // clang-format on

            template(typename I)(requires(!unsigned_integral<I>)) //
                void iota_advance_(I & i, iota_difference_t<I> n)
            {
                // TODO: bounds-check this - Eric Niebler
                i += n;
            }

            template(typename Int)(requires unsigned_integral<Int>) void iota_advance_(
                Int & i, iota_difference_t<Int> n)
            {
                // TODO: bounds-check this - Eric Niebler
                if(n >= 0)
                    i += static_cast<Int>(n);
                else
                    i -= static_cast<Int>(-n);
            }

            template(typename I)(requires advanceable_<I> AND(!integral<I>)) //
                iota_difference_t<I> iota_distance_(I const & i, I const & s)
            {
                return static_cast<iota_difference_t<I>>(s - i);
            }

            template(typename Int)(requires signed_integral<Int>)
                iota_difference_t<Int> iota_distance_(Int i0, Int i1)
            {
                // TODO: bounds-check this - Eric Niebler
                return static_cast<iota_difference_t<Int>>(
                    static_cast<iota_difference_t<Int>>(i1) -
                    static_cast<iota_difference_t<Int>>(i0));
            }

            template(typename Int)(requires unsigned_integral<Int>)
                iota_difference_t<Int> iota_distance_(Int i0, Int i1)
            {
                // TODO: bounds-check this - Eric Niebler
                return (i0 > i1) ? static_cast<iota_difference_t<Int>>(
                                       -static_cast<iota_difference_t<Int>>(i0 - i1))
                                 : static_cast<iota_difference_t<Int>>(i1 - i0);
            }
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{

        /// An iota view in a closed range
        template<typename From, typename To /* = From */>
        struct EARANGES_EMPTY_BASES closed_iota_view
          : view_facade<closed_iota_view<From, To>, finite>
        {
        private:
            friend range_access;

            From from_ = From();
            EARANGES_NO_UNIQUE_ADDRESS To to_ = To();

            struct cursor
            {
                using difference_type = detail::iota_difference_t<From>;

            private:
                friend range_access;
                From from_ = From();
                EARANGES_NO_UNIQUE_ADDRESS To to_ = To();
                bool done_ = false;

                From read() const
                {
                    EARANGES_EXPECT(!done_);
                    return from_;
                }
                void next()
                {
                    EARANGES_EXPECT(!done_);
                    if(from_ == to_)
                        done_ = true;
                    else
                        ++from_;
                }
                bool equal(default_sentinel_t) const
                {
                    return done_;
                }
                CPP_member
                auto equal(cursor const & that) const //
                    -> CPP_ret(bool)(requires equality_comparable<From>)
                {
                    return that.from_ == from_ && that.done_ == done_;
                }
                CPP_member
                auto prev() //
                    -> CPP_ret(void)(requires detail::decrementable_<From>)
                {
                    if(done_)
                        done_ = false;
                    else
                        --from_;
                }
                CPP_member
                auto advance(difference_type n) //
                    -> CPP_ret(void)(requires detail::advanceable_<From>)
                {
                    if(n > 0)
                    {
                        EARANGES_ENSURE(detail::iota_distance_(from_, to_) >= n - !done_);
                        detail::iota_advance_(
                            from_,
                            n - (done_ =
                                     (detail::iota_distance_(from_, to_) <= n - !done_)));
                    }
                    else if(n < 0)
                        detail::iota_advance_(from_, n + eastl::exchange(done_, false));
                }
                CPP_member
                auto distance_to(cursor const & that) const //
                    -> CPP_ret(difference_type)(requires detail::advanceable_<From>)
                {
                    using D = difference_type;
                    return static_cast<D>(detail::iota_distance_(from_, that.from_)) +
                           ((D)that.done_ - (D)done_);
                }
                CPP_member
                auto distance_to(default_sentinel_t) const //
                    -> CPP_ret(difference_type)(requires sized_sentinel_for<To, From>)
                {
                    return difference_type(to_ - from_) + !done_;
                }

            public:
                cursor() = default;
                constexpr cursor(From from, To to, bool done = false)
                  : from_(eastl::move(from))
                  , to_(eastl::move(to))
                  , done_(done)
                {}
            };

            cursor begin_cursor() const
            {
                return {from_, to_};
            }
            CPP_member
            auto end_cursor() const //
                -> CPP_ret(cursor)(requires same_as<From, To>)
            {
                return {to_, to_, true};
            }
            CPP_member
            auto end_cursor() const //
                -> CPP_ret(default_sentinel_t)(requires(!same_as<From, To>))
            {
                return {};
            }

            constexpr void check_bounds_(eastl::true_type)
            {
                EARANGES_EXPECT(from_ <= to_);
            }
            constexpr void check_bounds_(eastl::false_type)
            {}

        public:
            closed_iota_view() = default;
            constexpr closed_iota_view(meta::id_t<From> from, meta::id_t<To> to)
              : from_(eastl::move(from))
              , to_(eastl::move(to))
            {
                check_bounds_(meta::bool_<totally_ordered_with<From, To>>{});
            }
        };

        template<typename From, typename To>
        EARANGES_INLINE_VAR constexpr bool
            enable_borrowed_range<closed_iota_view<From, To>> = true;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename From,
                 typename To)(requires weakly_incrementable<From> AND copyable<To> AND(
            !integral<From> || !integral<To> ||
            eastl::is_signed<From>::value == eastl::is_signed<To>::value)) //
            closed_iota_view(From, To) -> closed_iota_view<From, To>;
#endif

        template<typename From, typename To /* = unreachable_sentinel_t*/>
        struct EARANGES_EMPTY_BASES iota_view
          : view_facade<iota_view<From, To>,
                        same_as<To, unreachable_sentinel_t> ? infinite
                        : eastl::is_integral<From>::value && eastl::is_integral<To>::value
                            ? finite
                            : unknown>
        {
        private:
            friend range_access;
            From from_ = From();
            EARANGES_NO_UNIQUE_ADDRESS To to_ = To();

            struct cursor;
            struct sentinel
            {
            private:
                friend struct cursor;
                EARANGES_NO_UNIQUE_ADDRESS To to_;

            public:
                sentinel() = default;
                constexpr explicit sentinel(To to)
                  : to_(eastl::move(to))
                {}
            };

            struct cursor
            {
                using difference_type = detail::iota_difference_t<From>;

            private:
                friend range_access;
                From from_;

                From read() const
                {
                    return from_;
                }
                void next()
                {
                    ++from_;
                }
                bool equal(sentinel const & that) const
                {
                    return from_ == that.to_;
                }
                CPP_member
                auto equal(cursor const & that) const //
                    -> CPP_ret(bool)(requires equality_comparable<From>)
                {
                    return that.from_ == from_;
                }
                CPP_member
                auto prev() //
                    -> CPP_ret(void)(requires detail::decrementable_<From>)
                {
                    --from_;
                }
                CPP_member
                auto advance(difference_type n) //
                    -> CPP_ret(void)(requires detail::advanceable_<From>)
                {
                    detail::iota_advance_(from_, n);
                }
                // Not to spec: TODO the relational operators will effectively be
                // constrained with Advanceable, but they should be constrained with
                // totally_ordered. Reimplement iota_view without view_facade or
                // basic_iterator.
                CPP_member
                auto distance_to(cursor const & that) const //
                    -> CPP_ret(difference_type)(requires detail::advanceable_<From>)
                {
                    return detail::iota_distance_(from_, that.from_);
                }
                // Extension: see https://github.com/ericniebler/stl2/issues/613
                CPP_member
                auto distance_to(sentinel const & that) const //
                    -> CPP_ret(difference_type)(requires sized_sentinel_for<To, From>)
                {
                    return that.to_ - from_;
                }

            public:
                cursor() = default;
                constexpr explicit cursor(From from)
                  : from_(eastl::move(from))
                {}
            };
            cursor begin_cursor() const
            {
                return cursor{from_};
            }
            CPP_auto_member auto CPP_fun(end_cursor)()(
                const //
                requires(same_as<To, unreachable_sentinel_t>))
            {
                return unreachable;
            }
            CPP_auto_member auto CPP_fun(end_cursor)()(
                const //
                requires(!same_as<To, unreachable_sentinel_t>))
            {
                return meta::conditional_t<same_as<From, To>, cursor, sentinel>{to_};
            }
            constexpr void check_bounds_(eastl::true_type)
            {
                EARANGES_EXPECT(from_ <= to_);
            }
            constexpr void check_bounds_(eastl::false_type)
            {}

        public:
#ifdef EARANGES_WORKAROUND_MSVC_934264
            constexpr
#endif // EARANGES_WORKAROUND_MSVC_934264
                iota_view() = default;
            constexpr explicit iota_view(From from)
              : from_(eastl::move(from))
            {}
            constexpr iota_view(meta::id_t<From> from, meta::id_t<To> to)
              : from_(eastl::move(from))
              , to_(eastl::move(to))
            {
                check_bounds_(meta::bool_<totally_ordered_with<From, To>>{});
            }
        };

        template<typename From, typename To>
        EARANGES_INLINE_VAR constexpr bool enable_borrowed_range<iota_view<From, To>> =
            true;

#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        template(typename From,
                 typename To)(requires weakly_incrementable<From> AND copyable<To> AND(
            !integral<From> || !integral<To> ||
            eastl::is_signed<From>::value == eastl::is_signed<To>::value)) //
            iota_view(From, To) -> iota_view<From, To>;
#endif

        // TODO: Figure out what to do with the deprecate function and what not

        namespace views
        {
            struct iota_fn
            {
                template(typename From)(
                    requires weakly_incrementable<From>) iota_view<From>
                operator()(From value) const
                {
                    return iota_view<From>{eastl::move(value)};
                }
                template(typename From, typename To)(
                    requires weakly_incrementable<From> AND copyable<To> AND
                        detail::weakly_equality_comparable_with_<From, To>
                            AND(!integral<From> || !integral<To> ||
                                eastl::is_signed<From>::value ==
                                    eastl::is_signed<To>::value)) //
                    iota_view<From, To>
                    operator()(From from, To to) const
                {
                    return {eastl::move(from), eastl::move(to)};
                }
            };

            struct closed_iota_fn
            {
                template(typename From, typename To)(
                    requires weakly_incrementable<From> AND copyable<To> AND
                        detail::weakly_equality_comparable_with_<From, To>
                            AND(!integral<From> || !integral<To> ||
                                eastl::is_signed<From>::value ==
                                    eastl::is_signed<To>::value)) //
                    closed_iota_view<From, To>
                    operator()(From from, To to) const
                {
                    return {eastl::move(from), eastl::move(to)};
                }
            };

            /// \relates iota_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(iota_fn, iota)

            /// \relates closed_iota_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(closed_iota_fn, closed_iota)

            /// # ranges::views::ints
            /// The ints view returns a range of monotonically increasing ints.
            ///
            /// ## Example
            /// \snippet example/view/ints.cpp ints example
            ///
            /// ### Output
            /// \include example/view/ints_golden.txt
            ///
            /// ## Syntax
            /// ```cpp
            /// auto output_range = ranges::views::ints(lower_bound, upper_bound);
            /// ```
            ///
            /// ## Parameters
            /// <pre><b>lower_bound</b></pre>
            ///   - Optional lower bound
            ///
            /// <pre><b>upper_bound</b></pre>
            ///   - Exclusive upper bound
            ///   - Required when `lower_bound` is specified
            ///     - To create an infinite range with a `lower_bound`, use
            ///     `ranges::unreachable` as the `upper_bound`
            ///
            /// <pre><b>output_range</b></pre>
            ///   - Range of monotonically increasing ints
            ///   - When an `upper_bound` is not specified, the range is quasi-infinite
            ///
            struct ints_fn : iota_view<int>
            {
                ints_fn() = default;

                template(typename Val)(requires integral<Val>) constexpr iota_view<Val>
                operator()(Val value, unreachable_sentinel_t) const
                {
                    return iota_view<Val>{value};
                }
                template(typename Val)(
                    requires integral<Val>) constexpr iota_view<Val, Val>
                operator()(Val from, Val to) const
                {
                    return {from, to};
                }
            };

            /// \relates ints_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(ints_fn, ints)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/satisfy_boost_range.hpp>
EARANGES_SATISFY_BOOST_RANGE(::ranges::closed_iota_view)
EARANGES_SATISFY_BOOST_RANGE(::ranges::iota_view)

EARANGES_DIAGNOSTIC_POP

#include <EARanges/detail/epilogue.hpp>

#endif
