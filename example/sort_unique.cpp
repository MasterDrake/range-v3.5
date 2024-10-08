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

///[sort_unique]
// Remove all non-unique elements from a container.

#include <iostream>
#include <EASTL/vector.h>

#include <EARanges/action/sort.hpp>
#include <EARanges/action/unique.hpp>
#include <EARanges/view/all.hpp>

#include "../test/eastl_utils.h"

int main()
{
    eastl::vector<int> vi{9, 4, 5, 2, 9, 1, 0, 2, 6, 7, 4, 5, 6, 5, 9, 2, 7,
                        1, 4, 5, 3, 8, 5, 0, 2, 9, 3, 7, 5, 7, 5, 5, 6, 1,
                        4, 3, 1, 8, 4, 0, 7, 8, 8, 2, 6, 5, 3, 4, 5};
    using namespace ranges;
    vi |= actions::sort | actions::unique;
    // prints: [0,1,2,3,4,5,6,7,8,9]
    std::cout << views::all(vi) << '\n';
}
///[sort_unique]
