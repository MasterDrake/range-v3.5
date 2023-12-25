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
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/take_exactly.hpp>
#include <EASTL/ranges/view/reverse.hpp>
#include <EASTL/ranges/utility/copy.hpp>
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

    auto rng0 = rgi | views::take_exactly(6);
    has_type<int &>(*begin(rng0));
    CPP_assert(view_<decltype(rng0)>);
    CPP_assert(common_range<decltype(rng0)>);
    CPP_assert(sized_range<decltype(rng0)>);
    CPP_assert(random_access_iterator<decltype(begin(rng0))>);
    ::check_equal(rng0, {0, 1, 2, 3, 4, 5});
    CHECK(size(rng0) == 6u);

    auto rng1 = rng0 | views::reverse;
    has_type<int &>(*begin(rng1));
    CPP_assert(view_<decltype(rng1)>);
    CPP_assert(common_range<decltype(rng1)>);
    CPP_assert(sized_range<decltype(rng1)>);
    CPP_assert(random_access_iterator<decltype(begin(rng1))>);
    ::check_equal(rng1, {5, 4, 3, 2, 1, 0});

    eastl::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | views::take_exactly(6) | views::reverse;
    has_type<int &>(*begin(rng2));
    CPP_assert(view_<decltype(rng2)>);
    CPP_assert(common_range<decltype(rng2)>);
    CPP_assert(sized_range<decltype(rng2)>);
    CPP_assert(random_access_iterator<decltype(begin(rng2))>);
    ::check_equal(rng2, {5, 4, 3, 2, 1, 0});

    eastl::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | views::take_exactly(6);
    has_type<int &>(*begin(rng3));
    CPP_assert(view_<decltype(rng3)>);
    CPP_assert(!common_range<decltype(rng3)>);
    CPP_assert(sized_range<decltype(rng3)>);
    CPP_assert(bidirectional_iterator<decltype(begin(rng3))>);
    CPP_assert(!random_access_iterator<decltype(begin(rng3))>);
    ::check_equal(rng3, {0, 1, 2, 3, 4, 5});

    auto rng4 = views::iota(10) | views::take_exactly(10);
    CPP_assert(view_<decltype(rng4)>);
    CPP_assert(common_range<decltype(rng4)>);
    CPP_assert(sized_range<decltype(rng4)>);
    static_assert(!ranges::is_infinite<decltype(rng4)>::value, "");
    ::check_equal(rng4, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    CHECK(size(rng4) == 10u);

    auto rng5 = views::iota(10) | views::take_exactly(10) | views::reverse;
    CPP_assert(view_<decltype(rng5)>);
    CPP_assert(common_range<decltype(rng5)>);
    CPP_assert(sized_range<decltype(rng5)>);
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {19, 18, 17, 16, 15, 14, 13, 12, 11, 10});
    CHECK(size(rng5) == 10u);

    {
        auto rng = debug_input_view<int const>{rgi} | views::take_exactly(6);
        ::check_equal(rng, {0, 1, 2, 3, 4, 5});
    }

    return test_result();
}
