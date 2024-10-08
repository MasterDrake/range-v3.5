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
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <algorithm>

// template<random_access_iterator Iter>
//   requires ShuffleIterator<Iter>
//         && LessThanComparable<Iter::value_type>
//   void
//   push_heap(Iter first, Iter last);

#include <EASTL/memory.h>
#include <EASTL/random.h>
#include <EASTL/algorithm.h>
#include <EASTL/heap.h>
#include <EASTL/functional.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/heap_algorithm.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

namespace
{
    eastl::default_random_engine gen;

    void test_basic(int N)
    {
        auto push_heap = make_testable_1(ranges::push_heap);

        int* ia = new int[N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        eastl::shuffle(ia, ia+N, gen);
        for (int i = 0; i <= N; ++i)
        {
            push_heap(ia, ia+i).check([&](int *r){CHECK(r == ia + i);});
            CHECK(eastl::is_heap(ia, ia+i));
        }
        delete[] ia;
    }

    void test_comp(int N)
    {
        auto push_heap = make_testable_1(ranges::push_heap);

        int* ia = new int[N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        eastl::shuffle(ia, ia+N, gen);
        for (int i = 0; i <= N; ++i)
        {
            push_heap(ia, ia+i, eastl::greater<int>()).check([&](int *r){CHECK(r == ia+i);});
            CHECK(eastl::is_heap(ia, ia+i, eastl::greater<int>()));
        }
        delete[] ia;
    }

    struct S
    {
        int i;
    };

    void test_proj(int N)
    {
        auto push_heap = make_testable_1(ranges::push_heap);

        S* ia = new S[N];
        int* ib = new int[N];
        for (int i = 0; i < N; ++i)
            ia[i].i = i;
        eastl::shuffle(ia, ia+N, gen);
        for (int i = 0; i <= N; ++i)
        {
            push_heap(ia, ia+i, eastl::greater<int>(), &S::i).check([&](S *r){CHECK(r == ia+i);});
            eastl::transform(ia, ia+i, ib, eastl::mem_fn(&S::i));
            CHECK(eastl::is_heap(ib, ib+i, eastl::greater<int>()));
        }
        delete[] ia;
        delete[] ib;
    }

    struct indirect_less
    {
        template<class P>
        bool operator()(const P& x, const P& y)
            {return *x < *y;}
    };

    void test_move_only(int N)
    {
        auto const push_heap = make_testable_1(ranges::push_heap);
        eastl::unique_ptr<int>* ia = new eastl::unique_ptr<int>[N];
        for (int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        eastl::shuffle(ia, ia+N, gen);
        for (int i = 0; i <= N; ++i)
        {
            push_heap(ia, ia+i, indirect_less()).check([&](eastl::unique_ptr<int> *r){CHECK(r == ia+i);});
            CHECK(eastl::is_heap(ia, ia+i, indirect_less()));
        }
        delete[] ia;
    }
}

constexpr bool test_constexpr()
{
    using namespace ranges;
    constexpr int N = 100;
    test::array<int, N> ia{{0}};
    for(int i = 0; i < N; ++i)
        ia[i] = i;
    for(int i = 0; i <= N; ++i)
    {
        STATIC_CHECK_RETURN(push_heap(make_subrange(begin(ia), begin(ia) + i),
                                      eastl::greater<int>()) == begin(ia) + i);
        STATIC_CHECK_RETURN(is_heap(begin(ia), begin(ia) + i, eastl::greater<int>()));
    }
    return true;
}

int main()
{
    test_basic(1000);
    test_comp(1000);
    test_proj(1000);
    test_move_only(1000);

    {
        int const N = 1000;
        S* ia = new S[N];
        int* ib = new int[N];
        for (int i = 0; i < N; ++i)
            ia[i].i = i;
        eastl::shuffle(ia, ia+N, gen);
        for (int i = 0; i <= N; ++i)
        {
            CHECK(ranges::push_heap(ranges::make_subrange(ia, ia+i), eastl::greater<int>(), &S::i) == ia+i);
            eastl::transform(ia, ia+i, ib, eastl::mem_fn(&S::i));
            CHECK(eastl::is_heap(ib, ib+i, eastl::greater<int>()));
        }
        delete[] ia;
        delete[] ib;
    }

    {
        STATIC_CHECK(test_constexpr());
    }

    return test_result();
}
