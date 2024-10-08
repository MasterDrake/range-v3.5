// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EARanges/algorithm/max.hpp>
#include <EARanges/view/subrange.hpp>
#include <EASTL/memory.h>
#include <EASTL/numeric.h>
#include <EASTL/random.h>
#include <EASTL/algorithm.h>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

namespace
{
    eastl::default_random_engine gen;

    template<class Iter, class Sent = Iter>
    void
    test_iter(Iter first, Sent last)
    {
        EARANGES_ENSURE(first != last);
        auto rng = ranges::make_subrange(first, last);
        auto v = ranges::max(rng);
        for (Iter i = first; i != last; ++i)
            CHECK(!(v < *i));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter(unsigned N)
    {
        EARANGES_ENSURE(N > 0);
        eastl::unique_ptr<int[]> a{new int[N]};
        eastl::iota(a.get(), a.get()+N, 0);
        eastl::shuffle(a.get(), a.get()+N, gen);
        test_iter(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter()
    {
        test_iter<Iter, Sent>(1);
        test_iter<Iter, Sent>(2);
        test_iter<Iter, Sent>(3);
        test_iter<Iter, Sent>(10);
        test_iter<Iter, Sent>(1000);
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp(Iter first, Sent last)
    {
        EARANGES_ENSURE(first != last);
        auto rng = ranges::make_subrange(first, last);
        auto comp = eastl::greater<int>();
        auto v = ranges::max(rng, comp);
        for (Iter i = first; i != last; ++i)
            CHECK(!comp(v, *i));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp(unsigned N)
    {
        EARANGES_ENSURE(N > 0);
        eastl::unique_ptr<int[]> a{new int[N]};
        eastl::iota(a.get(), a.get()+N, 0);
        eastl::shuffle(a.get(), a.get()+N, gen);
        test_iter_comp(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp()
    {
        test_iter_comp<Iter, Sent>(1);
        test_iter_comp<Iter, Sent>(2);
        test_iter_comp<Iter, Sent>(3);
        test_iter_comp<Iter, Sent>(10);
        test_iter_comp<Iter, Sent>(1000);
    }

    struct S
    {
        int i;
    };
}

int main()
{
    test_iter<InputIterator<const int*> >();
    test_iter<ForwardIterator<const int*> >();
    test_iter<BidirectionalIterator<const int*> >();
    test_iter<RandomAccessIterator<const int*> >();
    test_iter<const int*>();
    test_iter<InputIterator<const int*>, Sentinel<const int*>>();
    test_iter<ForwardIterator<const int*>, Sentinel<const int*>>();
    test_iter<BidirectionalIterator<const int*>, Sentinel<const int*>>();
    test_iter<RandomAccessIterator<const int*>, Sentinel<const int*>>();

    test_iter_comp<InputIterator<const int*> >();
    test_iter_comp<ForwardIterator<const int*> >();
    test_iter_comp<BidirectionalIterator<const int*> >();
    test_iter_comp<RandomAccessIterator<const int*> >();
    test_iter_comp<const int*>();
    test_iter_comp<InputIterator<const int*>, Sentinel<const int*>>();
    test_iter_comp<ForwardIterator<const int*>, Sentinel<const int*>>();
    test_iter_comp<BidirectionalIterator<const int*>, Sentinel<const int*>>();
    test_iter_comp<RandomAccessIterator<const int*>, Sentinel<const int*>>();

    // Works with projections?
    S s[] = {S{1},S{2},S{3},S{4},S{40},S{5},S{6},S{7},S{8},S{9}};
    S v = ranges::max(s, eastl::less<int>{}, &S::i);
    CHECK(v.i == 40);

    // Works with initializer_lists? (Regression test for #1004)
    CHECK(ranges::max({4,3,1,2,6,5}) == 6);

    return test_result();
}
