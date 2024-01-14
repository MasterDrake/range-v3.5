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

///[for_each_sequence]
// Use the for_each to print from various containers
// output
// vector:   1 2 3 4 5 6
// array:    1 2 3 4 5 6
// list:     1 2 3 4 5 6
// fwd_list: 1 2 3 4 5 6
// deque:    1 2 3 4 5 6

#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/queue.h>
#include <EASTL/slist.h>
#include <EASTL/stack.h>
#include <EASTL/vector.h>
#include <EARanges/algorithm/for_each.hpp> // specific includes
#include <iostream>
using std::cout;

#include "../test/eastl_utils.h"

//void * __cdecl operator new[](size_t size, const char * name, int flags,
//                              unsigned debugFlags, const char * file, int line)
//{
//    return new uint8_t[size];
//}
//
//void * __cdecl operator new[](size_t size, size_t alignement, size_t offset,
//                              const char * name, int flags, unsigned debugFlags,
//                              const char * file, int line)
//{
//    return new uint8_t[size];
//}

auto print = [](int i) { cout << i << ' '; };

int main()
{
    cout << "vector:   ";
    eastl::vector<int> v{1, 2, 3, 4, 5, 6};
    ranges::for_each(v, print); // 1 2 3 4 5 6

    cout << "\narray:    ";
    eastl::array<int, 6> a{1, 2, 3, 4, 5, 6};
    ranges::for_each(a, print);

    cout << "\nlist:     ";
    eastl::list<int> ll{1, 2, 3, 4, 5, 6};
    ranges::for_each(ll, print);

    cout << "\nfwd_list: ";
    eastl::slist<int> fl{1, 2, 3, 4, 5, 6};
    ranges::for_each(fl, print);

    cout << "\ndeque:    ";
    eastl::deque<int> d{1, 2, 3, 4, 5, 6};
    ranges::for_each(d, print);
    cout << '\n';
}
///[for_each_sequence]
