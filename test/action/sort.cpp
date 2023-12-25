// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <random>
#include <EASTL/vector.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/stride.hpp>
#include <EASTL/ranges/view/take.hpp>
#include <EASTL/ranges/algorithm/shuffle.hpp>
#include <EASTL/ranges/algorithm/copy.hpp>
#include <EASTL/ranges/algorithm/move.hpp>
#include <EASTL/ranges/algorithm/is_sorted.hpp>
#include <EASTL/ranges/algorithm/equal.hpp>
#include <EASTL/ranges/action/shuffle.hpp>
#include <EASTL/ranges/action/sort.hpp>
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

    auto v = views::ints(0,100) | to<eastl::vector>();
    v |= actions::shuffle(gen);
    CHECK(!is_sorted(v));

    auto v2 = v | copy | actions::sort;
    CHECK(size(v2) == size(v));
    CHECK(is_sorted(v2));
    CHECK(!is_sorted(v));
    CPP_assert(same_as<decltype(v), decltype(v2)>);

    v |= actions::sort;
    CHECK(is_sorted(v));

    v |= actions::shuffle(gen);
    CHECK(!is_sorted(v));

    v = v | move | actions::sort(eastl::less<int>());
    CHECK(is_sorted(v));
    CHECK(equal(v, v2));

    // Container algorithms can also be called directly
    // in which case they take and return by reference
    shuffle(v, gen);
    CHECK(!is_sorted(v));
    auto & v3 = actions::sort(v);
    CHECK(is_sorted(v));
    CHECK(&v3 == &v);

    auto r = views::ref(v);
    r |= actions::sort;

    // Can pipe a view to a "container" algorithm.
    actions::sort(v, eastl::greater<int>());
    v | views::stride(2) | actions::sort;
    check_equal(views::take(v, 10), {1,98,3,96,5,94,7,92,9,90});

    return ::test_result();
}
