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
#include <random>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/stable_sort.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

void * __cdecl operator new[](size_t size, const char * name, int flags,
                              unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset,
                              const char * name, int flags, unsigned debugFlags,
                              const char * file, int line)
{
    return new uint8_t[size];
}


namespace
{
    std::mt19937 gen;

#if !defined(__clang__) || !defined(_MSVC_STL_VERSION) // Avoid #890
    struct indirect_less
    {
        template<class P>
        bool operator()(const P& x, const P& y)
            {return *x < *y;}
    };
#endif // Avoid #890

    template<class RI>
    void
    test_sort_helper(RI f, RI l)
    {
        using value_type = ranges::iter_value_t<RI>;
        auto stable_sort = make_testable_1<false>(ranges::stable_sort);
        if (f != l)
        {
            auto len = l - f;
            value_type* save(new value_type[len]);
            do
            {
                eastl::copy(f, l, save);
                stable_sort(save, save+len).check([&](int *res)
                {
                    CHECK(res == save+len);
                    CHECK(eastl::is_sorted(save, save+len));
                    eastl::copy(f, l, save);
                });
                stable_sort(save, save+len, eastl::greater<int>{}).check([&](int *res)
                {
                    CHECK(res == save+len);
                    CHECK(eastl::is_sorted(save, save+len, eastl::greater<int>{}));
                    eastl::copy(f, l, save);
                });
            } while (eastl::next_permutation(f, l));
            delete [] save;
        }
    }

    template<class RI>
    void
    test_sort_driver_driver(RI f, RI l, int start, RI real_last)
    {
        for (RI i = l; i > f + start;)
        {
            *--i = start;
            if (f == i)
            {
                test_sort_helper(f, real_last);
            }
            if (start > 0)
                test_sort_driver_driver(f, i, start-1, real_last);
        }
    }

    template<class RI>
    void
    test_sort_driver(RI f, RI l, int start)
    {
        test_sort_driver_driver(f, l, start, l);
    }

    template<int sa>
    void
    test_sort_()
    {
        int ia[sa];
        for (int i = 0; i < sa; ++i)
        {
            test_sort_driver(ia, ia+sa, i);
        }
    }

    void
    test_larger_sorts(int N, int M)
    {
        EARANGES_ENSURE(N > 0);
        EARANGES_ENSURE(M > 0);
        // create array length N filled with M different numbers
        int* array = new int[N];
        int x = 0;
        for (int i = 0; i < N; ++i)
        {
            array[i] = x;
            if (++x == M)
                x = 0;
        }

        // test saw tooth pattern
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        // test random pattern
        std::shuffle(array, array+N, gen);
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        // test sorted pattern
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        // test reverse sorted pattern
        eastl::reverse(array, array+N);
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        // test swap ranges 2 pattern
        eastl::swap_ranges(array, array+N/2, array+N/2);
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        // test reverse swap ranges 2 pattern
        eastl::reverse(array, array+N);
        eastl::swap_ranges(array, array+N/2, array+N/2);
        CHECK(ranges::stable_sort(array, array+N) == array+N);
        CHECK(eastl::is_sorted(array, array+N));
        delete [] array;
    }

    void
    test_larger_sorts(unsigned N)
    {
        test_larger_sorts(N, 1);
        test_larger_sorts(N, 2);
        test_larger_sorts(N, 3);
        test_larger_sorts(N, N/2-1);
        test_larger_sorts(N, N/2);
        test_larger_sorts(N, N/2+1);
        test_larger_sorts(N, N-2);
        test_larger_sorts(N, N-1);
        test_larger_sorts(N, N);
    }

    struct S
    {
        int i, j;
    };
}

int main()
{
    // test null range
    int d = 0;
    int * r = ranges::stable_sort(&d, &d);
    CHECK(r == &d);
    // exhaustively test all possibilities up to length 8
    test_sort_<1>();
    test_sort_<2>();
    test_sort_<3>();
    test_sort_<4>();
    test_sort_<5>();
    test_sort_<6>();
    test_sort_<7>();
    test_sort_<8>();

    test_larger_sorts(15);
    test_larger_sorts(16);
    test_larger_sorts(17);
    test_larger_sorts(256);
    test_larger_sorts(257);
    test_larger_sorts(499);
    test_larger_sorts(500);
    test_larger_sorts(997);
    test_larger_sorts(1000);
    test_larger_sorts(1009);

#if !defined(__clang__) || !defined(_MSVC_STL_VERSION) // Avoid #890
    // Check move-only types
    {
        eastl::vector<eastl::unique_ptr<int> > v(1000);
        for(int i = 0; (std::size_t)i < v.size(); ++i)
            v[i].reset(new int((int)v.size() - i - 1));
        ranges::stable_sort(v, indirect_less());
        for(int i = 0; (std::size_t)i < v.size(); ++i)
            CHECK(*v[i] == i);
    }

    // Check projections
    {
        eastl::vector<S> v(1000, S{});
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            v[i].i = (int)v.size() - i - 1;
            v[i].j = i;
        }
        ranges::stable_sort(v, eastl::less<int>{}, &S::i);
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            CHECK(v[i].i == i);
            CHECK((std::size_t)v[i].j == v.size() - i - 1);
        }
    }

    // Check rvalue container
    {
        eastl::vector<S> v(1000, S{});
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            v[i].i = (int)v.size() - i - 1;
            v[i].j = i;
        }
        CHECK(::is_dangling(ranges::stable_sort(eastl::move(v), eastl::less<int>{}, &S::i)));
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            CHECK(v[i].i == i);
            CHECK((std::size_t)v[i].j == v.size() - i - 1);
        }
    }

    // Check rvalue forwarding range
    {
        eastl::vector<S> v(1000, S{});
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            v[i].i = (int)v.size() - i - 1;
            v[i].j = i;
        }
        CHECK(ranges::stable_sort(ranges::views::all(v), eastl::less<int>{}, &S::i) == v.end());
        for(int i = 0; (std::size_t)i < v.size(); ++i)
        {
            CHECK(v[i].i == i);
            CHECK((std::size_t)v[i].j == v.size() - i - 1);
        }
    }
#endif // Avoid #890

    return ::test_result();
}
