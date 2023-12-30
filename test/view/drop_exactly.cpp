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

#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/chunk.hpp>
#include <EARanges/view/drop_exactly.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/join.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/utility/copy.hpp>
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

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | views::drop_exactly(6);
    has_type<int &>(*begin(rng0));
    CPP_assert(view_<decltype(rng0)>);
    CPP_assert(common_range<decltype(rng0)>);
    CPP_assert(sized_range<decltype(rng0)>);
    CPP_assert(random_access_iterator<decltype(begin(rng0))>);
    ::check_equal(rng0, {6, 7, 8, 9, 10});
    CHECK(size(rng0) == 5u);

    auto rng1 = rng0 | views::reverse;
    has_type<int &>(*begin(rng1));
    CPP_assert(view_<decltype(rng1)>);
    CPP_assert(common_range<decltype(rng1)>);
    CPP_assert(sized_range<decltype(rng1)>);
    CPP_assert(random_access_iterator<decltype(begin(rng1))>);
    ::check_equal(rng1, {10, 9, 8, 7, 6});

    eastl::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | views::drop_exactly(6) | views::reverse;
    has_type<int &>(*begin(rng2));
    CPP_assert(view_<decltype(rng2)>);
    CPP_assert(common_range<decltype(rng2)>);
    CPP_assert(sized_range<decltype(rng2)>);
    CPP_assert(random_access_iterator<decltype(begin(rng2))>);
    ::check_equal(rng2, {10, 9, 8, 7, 6});

    eastl::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | views::drop_exactly(6);
    has_type<int &>(*begin(rng3));
    CPP_assert(view_<decltype(rng3)>);
    CPP_assert(common_range<decltype(rng3)>);
    CPP_assert(sized_range<decltype(rng3)>);
    CPP_assert(bidirectional_iterator<decltype(begin(rng3))>);
    CPP_assert(!random_access_iterator<decltype(begin(rng3))>);
    ::check_equal(rng3, {6, 7, 8, 9, 10});

    auto rng4 = views::iota(10) | views::drop_exactly(10);
    CPP_assert(view_<decltype(rng4)>);
    CPP_assert(!common_range<decltype(rng4)>);
    CPP_assert(!sized_range<decltype(rng4)>);
    static_assert(ranges::is_infinite<decltype(rng4)>::value, "");
    auto b = ranges::begin(rng4);
    CHECK(*b == 20);
    CHECK(*(b+1) == 21);

    auto rng5 = views::iota(10) | views::drop_exactly(10) | views::take(10) | views::reverse;
    CPP_assert(view_<decltype(rng5)>);
    CPP_assert(common_range<decltype(rng5)>);
    CPP_assert(sized_range<decltype(rng5)>);
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {29, 28, 27, 26, 25, 24, 23, 22, 21, 20});
    CHECK(size(rng5) == 10u);

    {
        // drop_exactly should work with random-access mutable-only Views.
        auto odds = views::iota(0) |
            views::chunk(2) |
            views::transform(views::drop_exactly(1)) |
            views::join;
        (void)odds;
    }

    {
        auto rng = debug_input_view<int const>{rgi} | views::drop_exactly(5);
        using Rng = decltype(rng);
        CPP_assert(input_range<Rng> && view_<Rng>);
        CPP_assert(sized_range<Rng>);
        ::check_equal(rng, {5,6,7,8,9,10});
    }

    return test_result();
}
