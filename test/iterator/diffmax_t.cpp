// Range v3 library
//
//  Copyright Eric Niebler 2019-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EARanges/iterator/diffmax_t.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

//#include <iomanip>
#include <EASTL/functional.h>
using ranges::detail::diffmax_t;

template<template<typename> class Op>
void check_1(std::ptrdiff_t a, std::ptrdiff_t b)
{
    // std::cout << std::dec;
    // std::cout << a << "&" << b << " == " << (a&b) << std::endl;
    // std::cout << std::hex;
    // std::cout << a << "&" << b << " == " << (a&b) << std::endl;
    CHECK(Op<diffmax_t>{}(a, b) == Op<std::ptrdiff_t>{}(a, b));
}
template<>
void check_1<eastl::divides>(std::ptrdiff_t a, std::ptrdiff_t b)
{
    if(b)
        CHECK(eastl::divides<diffmax_t>{}(a, b) == eastl::divides<std::ptrdiff_t>{}(a, b));
}
template<>
void check_1<eastl::modulus>(std::ptrdiff_t a, std::ptrdiff_t b)
{
    if(b)
        CHECK(eastl::modulus<diffmax_t>{}(a, b) == eastl::modulus<std::ptrdiff_t>{}(a, b));
}

template<template<typename> class Op>
void check()
{
    check_1<Op>(0, 0);
    check_1<Op>(-1, 0);
    check_1<Op>(0, -1);
    check_1<Op>(1, 0);
    check_1<Op>(0, 1);
    check_1<Op>(1, 1);
    check_1<Op>(-1, -1);
    check_1<Op>(-5, -4);
    check_1<Op>(-4, -5);
    check_1<Op>(5, -4);
    check_1<Op>(-4, 5);
    check_1<Op>(-5, 4);
    check_1<Op>(4, -5);
}

int main()
{
    check<eastl::plus>();
    check<eastl::minus>();
    check<eastl::multiplies>();
    check<eastl::divides>();
    check<eastl::modulus>();
    check<eastl::logical_and>();
    check<eastl::logical_or>();
   //TODO: eastl needs a logical xor check<eastl::bit_xor>();

    return test_result();
}
