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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EASTL/algorithm.h>
#include <EASTL/functional.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/fill.hpp>
#include <EARanges/algorithm/set_algorithm.hpp>
#include <EARanges/algorithm/lexicographical_compare.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class Iter1, class Iter2, class OutIter>
void
test()
{
    int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    static const int sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {2, 4, 4, 6};
    static const int sb = sizeof(ib)/sizeof(ib[0]);
    int ic[20];
    int ir[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6};
    static const int sr = sizeof(ir)/sizeof(ir[0]);

    using R = ranges::set_union_result<Iter1, Iter2, OutIter>;
    auto set_union = make_testable_2(ranges::set_union);

    auto checker = [&](R res)
    {
        CHECK((base(res.out) - ic) == sr);
        CHECK(eastl::lexicographical_compare(ic, base(res.out), ir, ir+sr) == false);
        ranges::fill(ic, 0);
    };

    set_union(Iter1(ia), Iter1(ia+sa),
        Iter2(ib), Iter2(ib+sb), OutIter(ic)).check(checker);
    set_union(Iter1(ib), Iter1(ib+sb),
        Iter2(ia), Iter2(ia+sa), OutIter(ic)).check(checker);

    set_union(Iter1(ia), Iter1(ia+sa),
        Iter2(ib), Iter2(ib+sb), OutIter(ic), eastl::less<int>()).check(checker);
    set_union(Iter1(ib), Iter1(ib+sb),
        Iter2(ia), Iter2(ia+sa), OutIter(ic), eastl::less<int>()).check(checker);
}

struct S
{
    int i;
};

struct T
{
    int j;
};

struct U
{
    int k;
    U& operator=(S s) { k = s.i; return *this;}
    U& operator=(T t) { k = t.j; return *this;}
};

constexpr bool test_constexpr()
{
    using namespace ranges;
    int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    int ib[] = {2, 4, 4, 6};
    int ic[20] = {0};
    int ir[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6};
    const int sr = sizeof(ir) / sizeof(ir[0]);

    const auto res1 = set_union(ia, ib, ic, less{});
    STATIC_CHECK_RETURN(res1.in1 == end(ia));
    STATIC_CHECK_RETURN(res1.in2 == end(ib));
    STATIC_CHECK_RETURN((res1.out - begin(ic)) == sr);
    STATIC_CHECK_RETURN(lexicographical_compare(ic, res1.out, ir, ir + sr, less{}) == 0);
    fill(ic, 0);

    const auto res2 = set_union(ib, ia, ic, less{});
    STATIC_CHECK_RETURN(res2.in1 == end(ib));
    STATIC_CHECK_RETURN(res2.in2 == end(ia));
    STATIC_CHECK_RETURN(res2.out - begin(ic) == sr);
    STATIC_CHECK_RETURN(lexicographical_compare(ic, res2.out, ir, ir + sr, less{}) == 0);

    return true;
}

int main()
{
#ifdef SET_UNION_1
    test<InputIterator<const int*>, InputIterator<const int*>, OutputIterator<int*> >();
    test<InputIterator<const int*>, InputIterator<const int*>, ForwardIterator<int*> >();
    test<InputIterator<const int*>, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<InputIterator<const int*>, InputIterator<const int*>, RandomAccessIterator<int*> >();
    test<InputIterator<const int*>, InputIterator<const int*>, int*>();

    test<InputIterator<const int*>, ForwardIterator<const int*>, OutputIterator<int*> >();
    test<InputIterator<const int*>, ForwardIterator<const int*>, ForwardIterator<int*> >();
    test<InputIterator<const int*>, ForwardIterator<const int*>, BidirectionalIterator<int*> >();
    test<InputIterator<const int*>, ForwardIterator<const int*>, RandomAccessIterator<int*> >();
    test<InputIterator<const int*>, ForwardIterator<const int*>, int*>();

    test<InputIterator<const int*>, BidirectionalIterator<const int*>, OutputIterator<int*> >();
    test<InputIterator<const int*>, BidirectionalIterator<const int*>, ForwardIterator<int*> >();
    test<InputIterator<const int*>, BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<InputIterator<const int*>, BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<InputIterator<const int*>, BidirectionalIterator<const int*>, int*>();

    test<InputIterator<const int*>, RandomAccessIterator<const int*>, OutputIterator<int*> >();
    test<InputIterator<const int*>, RandomAccessIterator<const int*>, ForwardIterator<int*> >();
    test<InputIterator<const int*>, RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<InputIterator<const int*>, RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<InputIterator<const int*>, RandomAccessIterator<const int*>, int*>();

    test<InputIterator<const int*>, const int*, OutputIterator<int*> >();
    test<InputIterator<const int*>, const int*, ForwardIterator<int*> >();
    test<InputIterator<const int*>, const int*, BidirectionalIterator<int*> >();
    test<InputIterator<const int*>, const int*, RandomAccessIterator<int*> >();
    test<InputIterator<const int*>, const int*, int*>();
#endif
#ifdef SET_UNION_2
    test<ForwardIterator<const int*>, InputIterator<const int*>, OutputIterator<int*> >();
    test<ForwardIterator<const int*>, InputIterator<const int*>, ForwardIterator<int*> >();
    test<ForwardIterator<const int*>, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<ForwardIterator<const int*>, InputIterator<const int*>, RandomAccessIterator<int*> >();
    test<ForwardIterator<const int*>, InputIterator<const int*>, int*>();

    test<ForwardIterator<const int*>, ForwardIterator<const int*>, OutputIterator<int*> >();
    test<ForwardIterator<const int*>, ForwardIterator<const int*>, ForwardIterator<int*> >();
    test<ForwardIterator<const int*>, ForwardIterator<const int*>, BidirectionalIterator<int*> >();
    test<ForwardIterator<const int*>, ForwardIterator<const int*>, RandomAccessIterator<int*> >();
    test<ForwardIterator<const int*>, ForwardIterator<const int*>, int*>();

    test<ForwardIterator<const int*>, BidirectionalIterator<const int*>, OutputIterator<int*> >();
    test<ForwardIterator<const int*>, BidirectionalIterator<const int*>, ForwardIterator<int*> >();
    test<ForwardIterator<const int*>, BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<ForwardIterator<const int*>, BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<ForwardIterator<const int*>, BidirectionalIterator<const int*>, int*>();

    test<ForwardIterator<const int*>, RandomAccessIterator<const int*>, OutputIterator<int*> >();
    test<ForwardIterator<const int*>, RandomAccessIterator<const int*>, ForwardIterator<int*> >();
    test<ForwardIterator<const int*>, RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<ForwardIterator<const int*>, RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<ForwardIterator<const int*>, RandomAccessIterator<const int*>, int*>();

    test<ForwardIterator<const int*>, const int*, OutputIterator<int*> >();
    test<ForwardIterator<const int*>, const int*, ForwardIterator<int*> >();
    test<ForwardIterator<const int*>, const int*, BidirectionalIterator<int*> >();
    test<ForwardIterator<const int*>, const int*, RandomAccessIterator<int*> >();
    test<ForwardIterator<const int*>, const int*, int*>();
#endif
#ifdef SET_UNION_3
    test<BidirectionalIterator<const int*>, InputIterator<const int*>, OutputIterator<int*> >();
    test<BidirectionalIterator<const int*>, InputIterator<const int*>, ForwardIterator<int*> >();
    test<BidirectionalIterator<const int*>, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, InputIterator<const int*>, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, InputIterator<const int*>, int*>();

    test<BidirectionalIterator<const int*>, ForwardIterator<const int*>, OutputIterator<int*> >();
    test<BidirectionalIterator<const int*>, ForwardIterator<const int*>, ForwardIterator<int*> >();
    test<BidirectionalIterator<const int*>, ForwardIterator<const int*>, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, ForwardIterator<const int*>, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, ForwardIterator<const int*>, int*>();

    test<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, OutputIterator<int*> >();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, ForwardIterator<int*> >();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, int*>();

    test<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, OutputIterator<int*> >();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, ForwardIterator<int*> >();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, int*>();

    test<BidirectionalIterator<const int*>, const int*, OutputIterator<int*> >();
    test<BidirectionalIterator<const int*>, const int*, ForwardIterator<int*> >();
    test<BidirectionalIterator<const int*>, const int*, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, const int*, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, const int*, int*>();
#endif
#ifdef SET_UNION_4
    test<RandomAccessIterator<const int*>, InputIterator<const int*>, OutputIterator<int*> >();
    test<RandomAccessIterator<const int*>, InputIterator<const int*>, ForwardIterator<int*> >();
    test<RandomAccessIterator<const int*>, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, InputIterator<const int*>, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, InputIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, ForwardIterator<const int*>, OutputIterator<int*> >();
    test<RandomAccessIterator<const int*>, ForwardIterator<const int*>, ForwardIterator<int*> >();
    test<RandomAccessIterator<const int*>, ForwardIterator<const int*>, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, ForwardIterator<const int*>, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, ForwardIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, OutputIterator<int*> >();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, ForwardIterator<int*> >();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, OutputIterator<int*> >();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, ForwardIterator<int*> >();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, const int*, OutputIterator<int*> >();
    test<RandomAccessIterator<const int*>, const int*, ForwardIterator<int*> >();
    test<RandomAccessIterator<const int*>, const int*, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, const int*, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, const int*, int*>();
#endif
#ifdef SET_UNION_5
    test<const int*, InputIterator<const int*>, OutputIterator<int*> >();
    test<const int*, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<const int*, InputIterator<const int*>, BidirectionalIterator<int*> >();
    test<const int*, InputIterator<const int*>, RandomAccessIterator<int*> >();
    test<const int*, InputIterator<const int*>, int*>();

    test<const int*, ForwardIterator<const int*>, OutputIterator<int*> >();
    test<const int*, ForwardIterator<const int*>, ForwardIterator<int*> >();
    test<const int*, ForwardIterator<const int*>, BidirectionalIterator<int*> >();
    test<const int*, ForwardIterator<const int*>, RandomAccessIterator<int*> >();
    test<const int*, ForwardIterator<const int*>, int*>();

    test<const int*, BidirectionalIterator<const int*>, OutputIterator<int*> >();
    test<const int*, BidirectionalIterator<const int*>, ForwardIterator<int*> >();
    test<const int*, BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<const int*, BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<const int*, BidirectionalIterator<const int*>, int*>();

    test<const int*, RandomAccessIterator<const int*>, OutputIterator<int*> >();
    test<const int*, RandomAccessIterator<const int*>, ForwardIterator<int*> >();
    test<const int*, RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<const int*, RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<const int*, RandomAccessIterator<const int*>, int*>();

    test<const int*, const int*, OutputIterator<int*> >();
    test<const int*, const int*, ForwardIterator<int*> >();
    test<const int*, const int*, BidirectionalIterator<int*> >();
    test<const int*, const int*, RandomAccessIterator<int*> >();
    test<const int*, const int*, int*>();
#endif
#ifdef SET_UNION_6
    // Test projections
    {
        S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        T ib[] = {T{2}, T{4}, T{4}, T{6}};
        U ic[20];
        int ir[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        using R = ranges::set_union_result<S *, T*, U*>;
        R res = ranges::set_union(ia, ib, ic, eastl::less<int>(), &S::i, &T::j);
        CHECK((res.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);
        ranges::fill(ic, U{0});

        using R2 = ranges::set_union_result<T *, S*, U*>;
        R2 res2 = ranges::set_union(ib, ia, ic, eastl::less<int>(), &T::j, &S::i);
        CHECK((res2.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res2.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);
    }

    // Test projections
#ifndef EARANGES_WORKAROUND_MSVC_573728
    {
        S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        T ib[] = {T{2}, T{4}, T{4}, T{6}};
        U ic[20];
        int ir[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        auto res = ranges::set_union(eastl::move(ia), ranges::views::all(ib), ic, eastl::less<int>(), &S::i, &T::j);
        CHECK(::is_dangling(res.in1));
        CHECK(res.in2 == ranges::end(ib));
        CHECK((res.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);

        ranges::fill(ic, U{0});
        auto res2 = ranges::set_union(eastl::move(ib), ranges::views::all(ia), ic, eastl::less<int>(), &T::j, &S::i);
        CHECK(res2.in2 == ranges::end(ia));
        CHECK(::is_dangling(res2.in1));
        CHECK((res2.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res2.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);
    }
#endif // EARANGES_WORKAROUND_MSVC_573728
    {
        eastl::vector<S> ia{S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        eastl::vector<T> ib{T{2}, T{4}, T{4}, T{6}};
        U ic[20];
        int ir[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        auto res = ranges::set_union(eastl::move(ia), ranges::views::all(ib), ic, eastl::less<int>(), &S::i, &T::j);
        CHECK(::is_dangling(res.in1));
        CHECK(res.in2 == ranges::end(ib));
        CHECK((res.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);

        ranges::fill(ic, U{0});
        auto res2 = ranges::set_union(eastl::move(ib), ranges::views::all(ia), ic, eastl::less<int>(), &T::j, &S::i);
        CHECK(res2.in2 == ranges::end(ia));
        CHECK(::is_dangling(res2.in1));
        CHECK((res2.out - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res2.out, ir, ir+sr, eastl::less<int>(), &U::k) == false);
    }

    {
        CHECK(test_constexpr());
    }
#endif

    return ::test_result();
}
