// Range v3 library
//
//  Copyright Eric Niebler 2017-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <sstream>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/tail.hpp>
#include <EASTL/ranges/view/empty.hpp>
#include <EASTL/ranges/view/single.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

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

    {
        eastl::vector<int> v{0,1,2,3};
        auto rng = views::tail(v);
        check_equal(rng, {1,2,3});
        CHECK(size(rng) == 3u);
    }

    {
        eastl::vector<int> v{};
        auto rng = views::tail(v);
        CHECK(empty(rng));
        CHECK(size(rng) == 0u);
    }

    {
        std::stringstream sin{"1 2 3 4"};
        istream_view<int> is(sin);
        auto rng = views::tail(is);
        check_equal(rng, {2,3,4});
    }

    {
        std::stringstream sin{""};
        istream_view<int> is(sin);
        auto rng = views::tail(is);
        CHECK(rng.begin() == rng.end());
    }

    {
        auto rng = views::empty<int> | views::tail;
        static_assert(0 == size(rng), "");
        CPP_assert(same_as<empty_view<int>, decltype(rng)>);
    }

    {
        tail_view<empty_view<int>> const rng(views::empty<int>);
        static_assert(0 == size(rng), "");
    }

    {
        auto const rng = views::single(1) | views::tail;
        static_assert(0 == size(rng), "");
    }

    return ::test_result();
}
