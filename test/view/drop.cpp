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
#include <EASTL/ranges/view/chunk.hpp>
#include <EASTL/ranges/view/drop.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/join.hpp>
#include <EASTL/ranges/view/reverse.hpp>
#include <EASTL/ranges/view/take.hpp>
#include <EASTL/ranges/view/transform.hpp>
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

    {
        auto rng0 = rgi | views::drop(6);
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
    }

    {
        eastl::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto rng2 = v | views::drop(6) | views::reverse;
        has_type<int &>(*begin(rng2));
        CPP_assert(view_<decltype(rng2)>);
        CPP_assert(common_range<decltype(rng2)>);
        CPP_assert(sized_range<decltype(rng2)>);
        CPP_assert(random_access_iterator<decltype(begin(rng2))>);
        ::check_equal(rng2, {10, 9, 8, 7, 6});
    }

    {
        eastl::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto rng3 = l | views::drop(6);
        has_type<int &>(*begin(rng3));
        CPP_assert(view_<decltype(rng3)>);
        CPP_assert(common_range<decltype(rng3)>);
        CPP_assert(sized_range<decltype(rng3)>);
        CPP_assert(bidirectional_iterator<decltype(begin(rng3))>);
        CPP_assert(!random_access_iterator<decltype(begin(rng3))>);
        ::check_equal(rng3, {6, 7, 8, 9, 10});
    }

    {
        auto rng4 = views::iota(10) | views::drop(10);
        CPP_assert(view_<decltype(rng4)>);
        CPP_assert(!common_range<decltype(rng4)>);
        CPP_assert(!sized_range<decltype(rng4)>);
        static_assert(ranges::is_infinite<decltype(rng4)>::value, "");
        auto b = ranges::begin(rng4);
        CHECK(*b == 20);
        CHECK(*(b+1) == 21);
    }

    {
        auto rng5 = views::iota(10) | views::drop(10) | views::take(10) | views::reverse;
        CPP_assert(view_<decltype(rng5)>);
        CPP_assert(common_range<decltype(rng5)>);
        CPP_assert(sized_range<decltype(rng5)>);
        static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
        ::check_equal(rng5, {29, 28, 27, 26, 25, 24, 23, 22, 21, 20});
        CHECK(size(rng5) == 10u);
    }

    {
        int some_ints[] = {0,1,2};
        auto rng = make_subrange(some_ints + 0, some_ints + 1);
        auto rng2 = views::drop(rng, 2);
        CHECK(begin(rng2) == some_ints + 1);
        CHECK(size(rng2) == 0u);
    }

    {
        // Regression test for https://github.com/ericniebler/range-v3/issues/413
        auto skips = [](eastl::vector<int> xs) {
            return views::ints(0, (int)xs.size())
                | views::transform([&](int n) {
                    return xs | views::chunk(n + 1)
                              | views::transform(views::drop(n))
                              | views::join;
                })
                | to<eastl::vector<eastl::vector<int>>>();
        };
        auto skipped = skips({1,2,3,4,5,6,7,8});
        CHECK(skipped.size() == 8u);
        if(skipped.size() >= 8u)
        {
            ::check_equal(skipped[0], {1,2,3,4,5,6,7,8});
            ::check_equal(skipped[1], {2,4,6,8});
            ::check_equal(skipped[2], {3,6});
            ::check_equal(skipped[3], {4,8});
            ::check_equal(skipped[4], {5});
            ::check_equal(skipped[5], {6});
            ::check_equal(skipped[6], {7});
            ::check_equal(skipped[7], {8});
        }
    }

    {
        static int const some_ints[] = {0,1,2,3};
        auto rng = debug_input_view<int const>{some_ints} | views::drop(2);
        using R = decltype(rng);
        CPP_assert(input_range<R> && view_<R>);
        CPP_assert(!forward_range<R>);
        CPP_assert(same_as<int const&, range_reference_t<R>>);
        ::check_equal(rng, {2,3});
    }

    {
        // regression test for #728
        auto rng1 = views::iota(1) | views::chunk(6) | views::take(3);
        int i = 2;
        RANGES_FOR(auto o1, rng1)
        {
            auto rng2 = o1 | views::drop(1);
            ::check_equal(rng2, {i, i+1, i+2, i+3, i+4});
            i += 6;
        }
    }

    {
        // regression test for #813
        static int const some_ints[] = {0,1,2,3};
        auto rng = some_ints | views::drop(10);
        CHECK(empty(rng));
    }

    return test_result();
}
