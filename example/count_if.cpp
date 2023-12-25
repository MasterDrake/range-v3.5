
// Range v3 library
//
//  Copyright Jeff Garland 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

///[count_if]
// This example counts element of a range that match a supplied predicate.
// output
// vector:   2
// array:    2

#include <iostream>
#include <EASTL/array.h>
#include <EASTL/vector.h>

#include <EASTL/ranges/algorithm/count_if.hpp> // specific includes
using std::cout;

auto is_six = [](int i) -> bool { return i == 6; };
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
int main()
{
    eastl::vector<int> v{6, 2, 3, 4, 5, 6};
    auto c = ranges::count_if(v, is_six);
    cout << "vector:   " << c << '\n'; // 2

    eastl::array<int, 6> a{6, 2, 6, 4, 5, 6};
    c = ranges::count_if(a, is_six);
    cout << "array:    " << c << '\n'; // 3
}
///[count_if]
