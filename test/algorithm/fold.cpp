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

#include <cmath>
#include <EASTL/functional.h>
#include <EASTL/string.h>

#include <EASTL/ranges/algorithm/fold.hpp>
#include <EASTL/ranges/algorithm/min.hpp>
#include <EASTL/ranges/core.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"

//#include <stdio.h>
//#include <stdarg.h>
//#include <wchar.h>
//#include <varargs.h>

//int __cdecl EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments)
//{
//    return vsnprintf_s(pDestination, n, _TRUNCATE, pFormat, arguments);
//}

void * __cdecl operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset,
                              const char * name, int flags, unsigned debugFlags,
                              const char * file, int line)
{
    return new uint8_t[size];
}

struct Approx
{
    double value;
    Approx(double v) : value(v)
    {}

    friend bool operator==(Approx a, double d)
    {
        return std::fabs(a.value - d) < 0.001;
    }
    friend bool operator==(double d, Approx a)
    {
        return a == d;
    }
};

template<class Iter, class Sent = Iter>
void test_left()
{
    double da[] = {0.25, 0.75};
    CHECK(ranges::fold_left(Iter(da), Sent(da), 1, eastl::plus<>()) == Approx{1.0});
    CHECK(ranges::fold_left(Iter(da), Sent(da + 2), 1, eastl::plus<>()) == Approx{2.0});

    CHECK(ranges::fold_left_first(Iter(da), Sent(da), ranges::min) == ranges::nullopt);
    CHECK(ranges::fold_left_first(Iter(da), Sent(da + 2), ranges::min) == ranges::optional<Approx>(0.25));

    using ranges::make_subrange;
    CHECK(ranges::fold_left(make_subrange(Iter(da), Sent(da)), 1, eastl::plus<>()) == Approx{1.0});
    CHECK(ranges::fold_left(make_subrange(Iter(da), Sent(da + 2)), 1, eastl::plus<>()) ==Approx{2.0});
    CHECK(ranges::fold_left_first(make_subrange(Iter(da), Sent(da)), ranges::min) == ranges::nullopt);
    CHECK(ranges::fold_left_first(make_subrange(Iter(da), Sent(da + 2)), ranges::min) == ranges::optional<Approx>(0.25));
}

void test_right()
{
    double da[] = {0.25, 0.75};
    CHECK(ranges::fold_right(da, da + 2, 1, eastl::plus<>()) == Approx{2.0});
    CHECK(ranges::fold_right(da, 1, eastl::plus<>()) == Approx{2.0});

    // f(0.25, f(0.75, 1))
    CHECK(ranges::fold_right(da, da + 2, 1, eastl::minus<>()) == Approx{0.5});
    CHECK(ranges::fold_right(da, 1, eastl::minus<>()) == Approx{0.5});

    int xs[] = {1, 2, 3};
    //TODO:11) Should be eastl::to_string but that requires implementing int __cdecl EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments) and i couldn't make it work.
    //Now I'm wondering where std::string comes from :/
    auto concat = [](int i, const eastl::string& s) { return s + std::to_string(i).c_str(); };
    CHECK(ranges::fold_right(xs, xs + 2, eastl::string(), concat) == "21");
    CHECK(ranges::fold_right(xs, eastl::string(), concat) == "321");
}

int main()
{
    test_left<InputIterator<const double *>>();
    test_left<ForwardIterator<const double *>>();
    test_left<BidirectionalIterator<const double *>>();
    test_left<RandomAccessIterator<const double *>>();
    test_left<const double *>();

    test_left<InputIterator<const double *>, Sentinel<const double *>>();
    test_left<ForwardIterator<const double *>, Sentinel<const double *>>();
    test_left<BidirectionalIterator<const double *>, Sentinel<const double *>>();
    test_left<RandomAccessIterator<const double *>, Sentinel<const double *>>();

    test_right();

    return ::test_result();
}
