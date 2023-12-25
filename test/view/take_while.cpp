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

#include <EASTL/vector.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/generate.hpp>
#include <EASTL/ranges/view/take_while.hpp>
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

struct my_data
{
    int i;
};

bool operator==(my_data left, my_data right)
{
    return left.i == right.i;
}

int main()
{
    using namespace ranges;
    auto rng0 = views::iota(10) | views::take_while([](int i) { return i != 25; });
    ::check_equal(rng0, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    CPP_assert(view_<decltype(rng0)>);
    CPP_assert(!common_range<decltype(rng0)>);
    CPP_assert(random_access_iterator<decltype(rng0.begin())>);

    eastl::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | views::take_while([](int i) { return i != 50; });
    CPP_assert(view_<decltype(rng1)>);
    CPP_assert(random_access_range<decltype(rng1)>);
    ::check_equal(rng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    // Check with a mutable predicate
    int rgi[] = {0,1,2,3,4,5,6,7,8,9};
    int cnt = 0;
    auto mutable_only = views::take_while(rgi, [cnt](int) mutable { return ++cnt <= 5;});
    ::check_equal(mutable_only, {0,1,2,3,4});
    CPP_assert(view_<decltype(mutable_only)>);
    CPP_assert(!view_<decltype(mutable_only) const>);

    {
        auto ns = views::generate([]() mutable {
            static int N;
            return ++N;
        });
        auto rng = ns | views::take_while([](int i) { return i < 5; });
        ::check_equal(rng, {1,2,3,4});
    }

    {
        auto ns = views::generate([]() mutable
        {
            static int N;
            return ++N;
        });
        auto rng = ns | views::take_while([](int i) mutable { return i < 5; });
        ::check_equal(rng, {1,2,3,4});
    }

    {
        auto rng = debug_input_view<int const>{rgi} | views::take_while([](int i) { return i != 5;});
        ::check_equal(rng, {0,1,2,3,4});
    }

    {
        auto ns = views::generate([]()
        {
            static int N;
            return my_data{++N};
        });
        auto rng = ns | views::take_while([](int i) { return i < 5; },  &my_data::i);
        ::check_equal(rng, eastl::vector<my_data>{{1},{2},{3},{4}});
    }

    return test_result();
}
