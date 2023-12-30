// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Rostislav Khlebnikov 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/for_each_n.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"


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

struct S
{
    void p() const { *p_ += i_; }
    int *p_;
    int i_;
};

constexpr bool test_constexpr()
{
    // lambda closure types are non-literal types before C++17
    struct
    {
        constexpr void operator()(int i)
        {
            sum_ += i;
        }

        int sum_ = 0;
    } fun;
    constexpr test::array<int, 5> v1{{1, 2, 4, 6}};
    STATIC_CHECK_RETURN(ranges::for_each_n(v1.begin(), 2, ranges::ref(fun)) ==
                        v1.begin() + 2);
    STATIC_CHECK_RETURN(ranges::for_each_n(v1, 2, ranges::ref(fun)) == v1.begin() + 2);
    STATIC_CHECK_RETURN(fun.sum_ == 3 * 2);
    return true;
}

int main()
{
    int sum = 0;
    auto const fun = [&](int i){ sum += i; };
    eastl::vector<int> v1 { 1, 2, 4, 6 };
    CHECK(ranges::for_each_n(v1.begin(), 2, fun) == v1.begin() + 2);
    CHECK(ranges::for_each_n(v1, 2, fun) == v1.begin() + 2);
    CHECK(sum == 3 * 2);

    sum = 0;
    auto const rfun = [&](int & i){ sum += i; };
    auto const sz = static_cast<int>(v1.size());
    CHECK(ranges::for_each_n(v1.begin(), sz, rfun) == v1.end());
    CHECK(ranges::for_each_n(v1, sz, rfun) == v1.end());
    CHECK(sum == 13 * 2);

    sum = 0;
    eastl::vector<S> v2{{&sum, 1}, {&sum, 2}, {&sum, 4}, {&sum, 6}};
    CHECK(ranges::for_each_n(v2.begin(), 3, &S::p) == v2.begin() + 3);
    CHECK(ranges::for_each_n(v2, 3, &S::p) == v2.begin() + 3);
    CHECK(sum == 7 * 2);

    sum = 0;
    CHECK(ranges::for_each_n(v2.begin(), 4, fun, &S::i_) == v2.begin() + 4);
    CHECK(ranges::for_each_n(v2, 4, fun, &S::i_) == v2.begin() + 4);
    CHECK(sum == 13 * 2);

    //TODO:13) Constepxr + eastl::addressof
    //STATIC_CHECK(test_constexpr());

    return ::test_result();
}
