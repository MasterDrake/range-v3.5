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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EASTL/memory.h>
#include <EASTL/random.h>
#include <EASTL/numeric.h>
#include <EASTL/algorithm.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/min_element.hpp>

#include "../array.hpp"
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
        Iter i = ranges::min_element(first, last);
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!(*j < *i));
        }
        else
            CHECK(i == last);

        auto rng = ::MakeTestRange(first, last);
        i = ranges::min_element(rng);
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!(*j < *i));
        }
        else
            CHECK(i == last);

        auto res = ranges::min_element(eastl::move(rng));
        CHECK(::is_dangling(res));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter(unsigned N)
    {
        eastl::unique_ptr<int[]> a{new int[N]};
        eastl::iota(a.get(), a.get()+N, 0);
        eastl::shuffle(a.get(), a.get()+N, gen);
        test_iter(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter()
    {
        test_iter<Iter, Sent>(0);
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
        Iter i = ranges::min_element(first, last, eastl::greater<int>());
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!eastl::greater<int>()(*j, *i));
        }
        else
            CHECK(i == last);

        auto rng = ::MakeTestRange(first, last);
        i = ranges::min_element(rng, eastl::greater<int>());
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!eastl::greater<int>()(*j, *i));
        }
        else
            CHECK(i == last);

        auto res = ranges::min_element(eastl::move(rng), eastl::greater<int>());
        CHECK(::is_dangling(res));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp(unsigned N)
    {
        eastl::unique_ptr<int[]> a{new int[N]};
        eastl::iota(a.get(), a.get()+N, 0);
        eastl::shuffle(a.get(), a.get()+N, gen);
        test_iter_comp(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp()
    {
        test_iter_comp<Iter, Sent>(0);
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
    test_iter<ForwardIterator<const int*> >();
    test_iter<BidirectionalIterator<const int*> >();
    test_iter<RandomAccessIterator<const int*> >();
    test_iter<const int*>();
    test_iter<ForwardIterator<const int*>, Sentinel<const int*>>();
    test_iter<BidirectionalIterator<const int*>, Sentinel<const int*>>();
    test_iter<RandomAccessIterator<const int*>, Sentinel<const int*>>();

    test_iter_comp<ForwardIterator<const int*> >();
    test_iter_comp<BidirectionalIterator<const int*> >();
    test_iter_comp<RandomAccessIterator<const int*> >();
    test_iter_comp<const int*>();
    test_iter_comp<ForwardIterator<const int*>, Sentinel<const int*>>();
    test_iter_comp<BidirectionalIterator<const int*>, Sentinel<const int*>>();
    test_iter_comp<RandomAccessIterator<const int*>, Sentinel<const int*>>();

    // Works with projections?
    S s[] = {S{1},S{2},S{3},S{4},S{-4},S{5},S{6},S{7},S{8},S{9}};
    S const *ps = ranges::min_element(s, eastl::less<int>{}, &S::i);
    CHECK(ps->i == -4);

    {
        constexpr auto a = test::array<int, 10>{{1, 2, 3, 4, -4, 5, 6, 7, 8, 9}};
        STATIC_CHECK(ranges::min_element(a) == ranges::begin(a) + 4);
    }

    return test_result();
}
