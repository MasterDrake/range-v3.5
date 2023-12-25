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

//[any_all_none_of]]
// Demonstrates any_of, all_of, none_of
// output
// vector: [6,2,3,4,5,6]
// vector any_of is_six: true
// vector all_of is_six: false
// vector none_of is_six: false

#include <EASTL/vector.h>
#include <iostream>

#include <EASTL/ranges/algorithm/all_of.hpp>
#include <EASTL/ranges/algorithm/any_of.hpp>
#include <EASTL/ranges/algorithm/for_each.hpp>
#include <EASTL/ranges/algorithm/none_of.hpp>
#include <EASTL/ranges/view/all.hpp>
using std::cout;

auto is_six = [](int i) { return i == 6; };

void * __cdecl operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

int main()
{
    eastl::vector<int> v{6, 2, 3, 4, 5, 6};
    cout << std::boolalpha;
    cout << "vector: " << ranges::views::all(v) << '\n';

    cout << "vector any_of is_six: " << ranges::any_of(v, is_six) << '\n';
    cout << "vector all_of is_six: " << ranges::all_of(v, is_six) << '\n';
    cout << "vector none_of is_six: " << ranges::none_of(v, is_six) << '\n';
}
//[any_all_none_of]]
