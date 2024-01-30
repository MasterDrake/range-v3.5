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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EASTL/array.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/algorithm/shuffle.hpp>
#include <EARanges/numeric/iota.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    constexpr unsigned N = 100;
    {
        eastl::array<int, N> a, b, c;
        for (auto p : {&a, &b, &c})
            ranges::iota(*p, 0);
        eastl::RNG<> g1, g2 = g1;
        ranges::shuffle(RandomAccessIterator<int*>(a.data()), Sentinel<int*>(a.data()+N), g1);
        CHECK(!ranges::equal(a, b));

        ranges::shuffle(b.begin(), b.end(), g1);
        CHECK(!ranges::equal(a, b));

        ranges::shuffle(c.begin(), c.end(), g2);
        CHECK(ranges::equal(a, c));
        CHECK(!ranges::equal(b, c));
    }

    {
        eastl::array<int, N> a, b, c;
        for (auto p : {&a, &b, &c})
            ranges::iota(*p, 0);
        eastl::RNG<> g1, g2 = g1;
        auto rng = ::MakeTestRange(RandomAccessIterator<int*>(a.data()), Sentinel<int*>(a.data() + N));
        ranges::shuffle(rng, g1);
        CHECK(!ranges::equal(a, b));

        ranges::shuffle(b, g2);
        CHECK(ranges::equal(a, b));

        ranges::shuffle(b, g1);
        CHECK(!ranges::equal(a, b));
        CHECK(!ranges::equal(b, c));

        ranges::iota(a, 0);
        ranges::shuffle(eastl::move(rng), g1);
        CHECK(!ranges::equal(a, c));
    }

    {
        eastl::array<int, N> a, b, c;
        for (auto p : {&a, &b, &c})
            ranges::iota(*p, 0);
        ranges::shuffle(RandomAccessIterator<int*>(a.data()), Sentinel<int*>(a.data() + N));
        CHECK(!ranges::equal(a, c));

        ranges::shuffle(b);
        CHECK(!ranges::equal(b, c));
        CHECK(!ranges::equal(a, b));
    }

    {
        eastl::array<int, N> a, b;
        for (auto p : {&a, &b})
            ranges::iota(*p, 0);
        ranges::shuffle(a);
        CHECK(!ranges::equal(a, b));
    }

    return ::test_result();
}
