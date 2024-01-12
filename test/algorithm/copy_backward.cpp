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

#include <cstring>
#include <EASTL/algorithm.h>
#include <EASTL/utility.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/copy_backward.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

void * __cdecl operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

constexpr bool test_constexpr()
{
    using IL = std::initializer_list<int>;
    constexpr test::array<int, 4> input{{0, 1, 2, 3}};
    test::array<int, 4> a1{{0, 0, 0, 0}};
    auto res = ranges::copy_backward(input, ranges::end(a1));
    STATIC_CHECK_RETURN(res.in == ranges::end(input));
    STATIC_CHECK_RETURN(res.out == ranges::begin(a1));
    STATIC_CHECK_RETURN(ranges::equal(a1, IL{0, 1, 2, 3}));
    return true;
}

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;

    eastl::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    eastl::pair<int, int> out[size(a)] = {};

    {
        auto res = ranges::copy_backward(begin(a), end(a), end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(eastl::equal(a, a + size(a), out));
    }

    {
        eastl::fill_n(out, size(out), eastl::make_pair(0, 0));
        auto res = ranges::copy_backward(a, end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(eastl::equal(a, a + size(a), out));
    }

#ifndef EARANGES_WORKAROUND_MSVC_573728
    {
        eastl::fill_n(out, size(out), eastl::make_pair(0, 0));
        auto res = ranges::copy_backward(eastl::move(a), end(out));
        CHECK(::is_dangling(res.in));
        CHECK(res.out == begin(out));
        CHECK(eastl::equal(a, a + size(a), out));
    }
#endif

    {
        eastl::fill_n(out, size(out), eastl::make_pair(0, 0));
        eastl::vector<eastl::pair<int, int>> vec(begin(a), end(a));
        auto res = ranges::copy_backward(eastl::move(vec), end(out));
        CHECK(::is_dangling(res.in));
        CHECK(res.out == begin(out));
        CHECK(eastl::equal(a, a + size(a), out));
    }

    {
        eastl::fill_n(out, size(out), eastl::make_pair(0, 0));
        auto res = ranges::copy_backward(ranges::views::all(a), end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(eastl::equal(a, a + size(a), out));
    }

    {
        STATIC_CHECK(test_constexpr());
    }
    return test_result();
}
