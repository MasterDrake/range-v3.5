// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/action/stride.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    auto v = views::ints(0,100) | to<eastl::vector>();

    auto v2 = v | copy | actions::stride(10);
    CHECK(size(v2) == 10u);
    CPP_assert(same_as<decltype(v), decltype(v2)>);
    ::check_equal(v2, {0,10,20,30,40,50,60,70,80,90});

    v2 = v2 | move | actions::stride(4);
    ::check_equal(v2, {0,40,80});

    v2 |= actions::stride(2);
    ::check_equal(v2, {0,80});
    v2 |= actions::stride(1);
    ::check_equal(v2, {0,80});
    v2 |= actions::stride(10);
    ::check_equal(v2, {0});

    auto & v3 = actions::stride(v, 30);
    CHECK(&v3 == &v);
    ::check_equal(v, {0,30,60,90});

    return ::test_result();
}
