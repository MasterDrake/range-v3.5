// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <random>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/repeat_n.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/algorithm/shuffle.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/algorithm/is_sorted.hpp>
#include <EARanges/action/shuffle.hpp>
#include <EARanges/action/sort.hpp>
#include <EARanges/action/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"


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

int main()
{
    using namespace ranges;
    std::mt19937 gen;

    // [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...]
    auto v = views::for_each(views::ints(1,100), [](int i)
        {
            return yield_from(views::repeat_n(i,i));
        }) | to<eastl::vector>();
    check_equal(views::take(v, 15), {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5});
    v |= actions::shuffle(gen);
    CHECK(!is_sorted(v));

    v |= actions::sort | actions::unique;
    CHECK(equal(v, views::ints(1,100)));

    return ::test_result();
}
