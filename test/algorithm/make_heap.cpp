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

#include <EASTL/memory.h>
#include <random>
//#include <EASTL/algorithm.h>
#include <EASTL/heap.h>
#include <EASTL/functional.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/heap_algorithm.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

//TODO: random and shuffle ...
EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

namespace
{
    std::mt19937 gen;

    void test_1(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N));
        delete [] ia;
    }

    void test_2(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, Sentinel<int*>(ia+N)) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N));
        delete [] ia;
    }

    void test_3(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_subrange(ia, ia+N)) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N));

        std::shuffle(ia, ia+N, gen);
        CHECK(::is_dangling(ranges::make_heap(::MakeTestRange(ia, ia+N))));
        CHECK(eastl::is_heap(ia, ia+N));

        delete [] ia;
    }

    void test_4(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_subrange(ia, Sentinel<int*>(ia+N))) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N));

        std::shuffle(ia, ia+N, gen);
        CHECK(::is_dangling(ranges::make_heap(::MakeTestRange(ia, Sentinel<int*>(ia+N)))));
        CHECK(eastl::is_heap(ia, ia+N));

        delete [] ia;
    }

    void test_5(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N, eastl::greater<int>()) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));
        delete [] ia;
    }

    void test_6(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, Sentinel<int*>(ia+N), eastl::greater<int>()) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));
        delete [] ia;
    }

    void test_7(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_subrange(ia, ia+N), eastl::greater<int>()) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));

        std::shuffle(ia, ia+N, gen);
        CHECK(::is_dangling(ranges::make_heap(::MakeTestRange(ia, ia+N), eastl::greater<int>())));
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));

        delete [] ia;
    }

    void test_8(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_subrange(ia, Sentinel<int*>(ia+N)), eastl::greater<int>()) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));

        std::shuffle(ia, ia+N, gen);
        CHECK(::is_dangling(ranges::make_heap(::MakeTestRange(ia, Sentinel<int*>(ia+N)), eastl::greater<int>())));
        CHECK(eastl::is_heap(ia, ia+N, eastl::greater<int>()));

        delete [] ia;
    }

    struct indirect_less
    {
        template<class P>
        bool operator()(const P& x, const P& y)
            {return *x < *y;}
    };

    void test_9(int N)
    {
        eastl::unique_ptr<int>* ia = new eastl::unique_ptr<int> [N];
        for (int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N, indirect_less()) == ia+N);
        CHECK(eastl::is_heap(ia, ia+N, indirect_less()));
        delete [] ia;
    }

    struct S
    {
        int i;
    };

    void test_10(int N)
    {
        int* ia = new int [N];
        S* ib = new S [N];
        for (int i = 0; i < N; ++i)
            ib[i].i = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ib, ib+N, eastl::less<int>(), &S::i) == ib+N);
        eastl::transform(ib, ib+N, ia, eastl::mem_fn(&S::i));
        CHECK(eastl::is_heap(ia, ia+N));
        delete [] ia;
        delete [] ib;
    }

    void test_all(int N)
    {
        test_1(N);
        test_2(N);
        test_3(N);
        test_4(N);
        test_5(N);
        test_6(N);
        test_7(N);
        test_8(N);
    }

    constexpr bool test_constexpr()
    {
        using namespace ranges;
        constexpr int N = 100;
        test::array<int, N> ia{{0}};
        for(int i = 0; i < N; ++i)
            ia[i] = N - 1 - i;
        STATIC_CHECK_RETURN(make_heap(begin(ia), end(ia), eastl::less<int>{}) == end(ia));
        STATIC_CHECK_RETURN(is_heap(begin(ia), end(ia)));
        return true;
    }
}

int main()
{
    test_all(0);
    test_all(1);
    test_all(2);
    test_all(3);
    test_all(10);
    test_all(1000);
    test_9(1000);
    test_10(1000);
    //TODO:17) eastl::addressof vs constexpr
    //{
    //    STATIC_CHECK(test_constexpr());
    //}

    return test_result();
}
