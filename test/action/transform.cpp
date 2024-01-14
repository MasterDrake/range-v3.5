// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <EARanges/action/transform.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EASTL/vector.h>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    auto v = views::ints(0, 10) | to<eastl::vector>();

    auto v0 = v | copy | actions::transform([](int i) { return i * i; });
    CPP_assert(same_as<decltype(v), decltype(v0)>);
    ::check_equal(v0, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81});

    actions::transform(v, [](int i) { return i * i; });
    ::check_equal(v, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81});

    return ::test_result();
}
