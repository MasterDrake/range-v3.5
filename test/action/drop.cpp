// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/action/drop.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    auto v = views::ints(1,21) | to<eastl::vector>();
    auto & v2 = actions::drop(v, 3);
    CHECK(&v2 == &v);
    CHECK(v.size() == 17u);
    CHECK(v[0] == 4);

    v = eastl::move(v) | actions::drop(3);
    CHECK(v.size() == 14u);
    CHECK(v[0] == 7);

    v |= actions::drop(3);
    CHECK(v.size() == 11u);
    CHECK(v[0] == 10);

    v |= actions::drop(100);
    CHECK(v.size() == 0u);

    return ::test_result();
}
