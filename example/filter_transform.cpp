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

///[filter_transform]
// This example demonstrates filtering and transforming a range on the fly with view adaptors.

#include <iostream>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include <EARanges/view/filter.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/range/conversion.hpp>
#include <EARanges/view/all.hpp>

#include <cstdio>

using std::cout;
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

EASTL_EASTDC_API int __cdecl EA::StdC::Vsnprintf(char*  EA_RESTRICT pDestination, size_t n, const char*  EA_RESTRICT pFormat, va_list arguments)
{
    return vsnprintf(pDestination, n, pFormat, arguments);
}

int main()
{
    eastl::vector<int> const vi{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    using namespace ranges;
    auto rng = vi | views::filter([](int i) { return i % 2 == 0; }) | views::transform([](int i) { return eastl::to_string(i); }) | to_vector;
    // prints: [2,4,6,8,10]
    std::cout << views::all(rng);
}
///[filter_transform]
