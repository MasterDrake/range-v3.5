// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/stride.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/action/slice.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"


void * __cdecl operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

int main()
{
    using namespace ranges;
    {
        auto v = views::ints(0, 100) | to<eastl::vector>();

        auto v2 = v | copy | actions::slice(10, 20);
        CHECK(size(v2) == 10u);
        CPP_assert(same_as<decltype(v), decltype(v2)>);
        ::check_equal(v2, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19});

        v2 = v2 | move | actions::slice(2, 8);
        ::check_equal(v2, {12, 13, 14, 15, 16, 17});

        v2 |= actions::slice(0, 0);
        CHECK(v2.size() == 0u);

        auto &v3 = actions::slice(v, 90, 100);
        CHECK(&v3 == &v);
        ::check_equal(v, {90, 91, 92, 93, 94, 95, 96, 97, 98, 99});
    }

    {
        auto rng = views::ints(0, 100) | to<eastl::vector>();

        rng |= actions::slice(20, end - 70);
        CHECK(size(rng) == 10u);
        ::check_equal(rng, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29});

        rng |= actions::slice(end - 10, end - 5);
        CHECK(size(rng) == 5u);
        ::check_equal(rng, {20, 21, 22, 23, 24});
    }

    {
        auto rng = views::ints(0, 100) | to<eastl::vector>();

        auto &rng_copy = actions::slice(rng, 90, end);
        CHECK(&rng_copy == &rng);
        CHECK(size(rng_copy) == 10u);
        ::check_equal(rng, {90, 91, 92, 93, 94, 95, 96, 97, 98, 99});

        rng |= actions::slice(end - 5, end);
        CHECK(&rng_copy == &rng);
        CHECK(size(rng_copy) == 5u);
        ::check_equal(rng, {95, 96, 97, 98, 99});
    }

    return ::test_result();
}
