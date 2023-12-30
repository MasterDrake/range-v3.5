/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <EASTL/vector.h>

#include <EARanges/view/remove.hpp>
#include <EARanges/view/remove_if.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

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

void test_straight()
{
    eastl::vector<int> vec = {1,2,3,4,5};
    auto out = vec | views::remove(2);

    ::check_equal(out, {1,3,4,5});
}

struct Int
{
    int i;
};
bool operator==(Int left, Int right)
{
    return left.i == right.i;
}
void test_proj()
{
    const eastl::vector<Int> vec{ Int{1}, Int{2}, Int{3}, Int{4}, Int{5} };
    auto out = vec | views::remove(2, &Int::i);

    ::check_equal(out, {Int{1}, Int{3}, Int{4}, Int{5}});
}

int main()
{
    // simple interface tests.
    // All other already tested in remove_if.
    test_straight();
    test_proj();

    return test_result();
}
