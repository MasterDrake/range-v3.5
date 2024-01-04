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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <EASTL/utility.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/lower_bound.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
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

struct my_int
{
    int value;
};

bool compare(my_int lhs, my_int rhs)
{
    return lhs.value < rhs.value;
}

void not_totally_ordered()
{
    // This better compile!
    eastl::vector<my_int> vec;
    ranges::lower_bound(vec, my_int{10}, compare);
}

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;
    using ranges::less;

    constexpr eastl::pair<int, int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    constexpr const eastl::pair<int, int> c[] = {
        {0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

    CHECK(ranges::aux::lower_bound_n(begin(a), size(a), a[0]) == &a[0]);
    CHECK(ranges::aux::lower_bound_n(begin(a), size(a), a[1], less()) == &a[1]);
    CHECK(ranges::aux::lower_bound_n(begin(a), size(a), 1, less(), &eastl::pair<int, int>::first) == &a[2]);

    CHECK(ranges::lower_bound(begin(a), end(a), a[0]) == &a[0]);
    CHECK(ranges::lower_bound(begin(a), end(a), a[1], less()) == &a[1]);
    CHECK(ranges::lower_bound(begin(a), end(a), 1, less(), &eastl::pair<int, int>::first) == &a[2]);

    CHECK(ranges::lower_bound(a, a[2]) == &a[2]);
    CHECK(ranges::lower_bound(c, c[3]) == &c[3]);

    CHECK(ranges::lower_bound(a, a[4], less()) == &a[4]);
    CHECK(ranges::lower_bound(c, c[5], less()) == &c[5]);

    CHECK(ranges::lower_bound(a, 1, less(), &eastl::pair<int, int>::first) == &a[2]);
    CHECK(ranges::lower_bound(c, 1, less(), &eastl::pair<int, int>::first) == &c[2]);

    CHECK(ranges::lower_bound(ranges::views::all(a), 1, less(), &eastl::pair<int, int>::first) == &a[2]);
    CHECK(ranges::lower_bound(ranges::views::all(c), 1, less(), &eastl::pair<int, int>::first) == &c[2]);
#ifndef EARANGES_WORKAROUND_MSVC_573728
    CHECK(::is_dangling(ranges::lower_bound(eastl::move(a), 1, less(), &eastl::pair<int, int>::first)));
    CHECK(::is_dangling(ranges::lower_bound(eastl::move(c), 1, less(), &eastl::pair<int, int>::first)));
#endif // EARANGES_WORKAROUND_MSVC_573728
    {
        eastl::vector<eastl::pair<int, int>> vec_a(ranges::begin(a), ranges::end(a));
        CHECK(::is_dangling(ranges::lower_bound(eastl::move(vec_a), 1, less(), &eastl::pair<int, int>::first)));
    }
    {
        eastl::vector<eastl::pair<int, int>> const vec_c(ranges::begin(c), ranges::end(c));
        CHECK(::is_dangling(ranges::lower_bound(eastl::move(vec_c), 1, less(), &eastl::pair<int, int>::first)));
    }

    {
        using namespace ranges;

        STATIC_CHECK(aux::lower_bound_n(begin(a), size(a), a[0]) == &a[0]);
        STATIC_CHECK(aux::lower_bound_n(begin(a), size(a), a[1], less()) == &a[1]);

        STATIC_CHECK(lower_bound(begin(a), end(a), a[0]) == &a[0]);
        STATIC_CHECK(lower_bound(begin(a), end(a), a[1], less()) == &a[1]);
        STATIC_CHECK(lower_bound(a, a[2]) == &a[2]);
        STATIC_CHECK(lower_bound(a, a[4], less()) == &a[4]);
        STATIC_CHECK(lower_bound(a, eastl::make_pair(1, 2), less()) == &a[2]);
#if EARANGES_CXX_CONSTEXPR >= EARANGES_CXX_CONSTEXPR_17
#if EASTL_ADDRESSOF_CONSTEXPR
        STATIC_CHECK(lower_bound(views::all(a), eastl::make_pair(1, 2), less()) == &a[2]);
#endif
#endif
    }

    return test_result();
}
