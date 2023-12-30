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
#include <EARanges/core.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/repeat.hpp>
#include <EARanges/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    auto rng = views::repeat(9) | views::take(10);
    CPP_assert(view_<decltype(rng)>);
    CPP_assert(sized_range<decltype(rng)>);
    CPP_assert(random_access_iterator<decltype(rng.begin())>);
    ::check_equal(rng, {9, 9, 9, 9, 9, 9, 9, 9, 9, 9});

    return test_result();
}
