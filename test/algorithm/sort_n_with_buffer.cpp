/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#include <EARanges/algorithm/aux_/sort_n_with_buffer.hpp>
#include <EASTL/functional.h>
#include <iostream>
#include <EASTL/utility.h>
#include <EARanges/range/primitives.hpp>
#include "../simple_test.hpp"

namespace eastl
{
    template<typename F, typename S>
    std::ostream& operator<<(std::ostream& os, eastl::pair<F, S> const& p)
    {
        return os << '{' << p.first << ", " << p.second << '}';
    }
}

int main()
{
    eastl::pair<int, int> some_pairs[] =
    {
        {0, 3}, {1, 2}, {2, 1}, {3, 0}
    };
    eastl::pair<int, int> space[2];

    ranges::aux::sort_n_with_buffer(some_pairs + 0, ranges::size(some_pairs), space + 0, eastl::less<int>{}, &eastl::pair<int, int>::second);
    CHECK(some_pairs[0] == eastl::make_pair(3, 0));
    CHECK(some_pairs[1] == eastl::make_pair(2, 1));
    CHECK(some_pairs[2] == eastl::make_pair(1, 2));
    CHECK(some_pairs[3] == eastl::make_pair(0, 3));

    ranges::aux::sort_n_with_buffer(some_pairs + 0, ranges::size(some_pairs), space + 0, eastl::less<int>{}, &eastl::pair<int, int>::first);
    CHECK(some_pairs[0] == eastl::make_pair(0, 3));
    CHECK(some_pairs[1] == eastl::make_pair(1, 2));
    CHECK(some_pairs[2] == eastl::make_pair(2, 1));
    CHECK(some_pairs[3] == eastl::make_pair(3, 0));

    return ::test_result();
}
