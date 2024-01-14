
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

///[for_each_assoc]
// for_each with associative containers
// output
// set:           1 2 3 4 5 6
// map:           one:1 three:3 two:2
// unordered_map: three:3 one:1 two:2
// unordered_set: 6 5 4 3 2 1

#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EARanges/algorithm/for_each.hpp>
#include <iostream>
using eastl::string;
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
auto print = [](int i) { cout << i << ' '; };
// must take a pair for map types
auto printm = [](eastl::pair<string, int> p)
{
    cout << p.first.c_str() << ":" << p.second << ' ';
};

int main()
{
    cout << "set:           ";
    eastl::set<int> si{1, 2, 3, 4, 5, 6};
    ranges::for_each(si, print);

    cout << "\nmap:           ";
    eastl::map<string, int> msi{{"one", 1}, {"two", 2}, {"three", 3}};
    ranges::for_each(msi, printm);

    cout << "\nunordered map: ";
    eastl::unordered_map<string, int> umsi{{"one", 1}, {"two", 2}, {"three", 3}};
    ranges::for_each(umsi, printm);

    cout << "\nunordered set: ";
    eastl::unordered_set<int> usi{1, 2, 3, 4, 5, 6};
    ranges::for_each(usi, print);
    cout << '\n';
}
///[for_each_assoc]
