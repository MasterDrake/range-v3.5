// Range v3 library
//
//  Copyright Eric Niebler 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

///[comprehension_conversion]
// Use a range comprehension (views::for_each) to construct a custom range, and then convert it to a eastl::vector.

#include <iostream>
#include <EASTL/vector.h>

#include <EARanges/range/conversion.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/repeat_n.hpp>
using std::cout;

#include "../test/eastl_utils.h"
int main()
{
    using namespace ranges;
    auto vi = views::for_each(views::ints(1, 6), [](int i) { return yield_from(views::repeat_n(i, i)); }) | to<eastl::vector>();
    // prints: [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5]
    cout << views::all(vi) << '\n';
}
///[comprehension_conversion]
