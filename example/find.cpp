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

///[find]
// vector:   *i: 6
// didn't find 10
// *i: 6
// *i: 2
// *i after ++ (2 expected): 2
// array:    *i: 6
// list:     *i: 6
// fwd_list: *i: 4
// deque:    *i: 6

#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/slist.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <iostream>

#include <EARanges/all.hpp>
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
    cout << "vector:   ";

    eastl::vector<int> v{6, 2, 6, 4, 6, 1};
    {
        auto i = ranges::find(v, 6); // 1 2 3 4 5 6
        cout << "*i: " << *i << '\n';
    }
    {
        auto i = ranges::find(v, 10); // 1 2 3 4 5 6
        if(i == ranges::end(v))
        {
            cout << "didn't find 10\n";
        }
    }
    {
        auto i = ranges::find_if(v, is_six);
        if(i != ranges::end(v))
        {
            cout << "*i: " << *i << '\n';
        }
    }
    {
        auto i = ranges::find_if_not(v, is_six);
        if(i != ranges::end(v))
        {
            cout << "*i: " << *i << '\n';
        }
    }
    {
        auto i = ranges::find(v, 6);
        i++;
        if(i != ranges::end(v))
        {
            cout << "*i after ++ (2 expected): " << *i;
        }
    }

    cout << "\narray:    ";

    eastl::array<int, 6> a{6, 2, 3, 4, 5, 1};
    {
        auto i = ranges::find(a, 6);
        if(i != ranges::end(a))
        {
            cout << "*i: " << *i;
        }
    }
    cout << "\nlist:     ";

    eastl::list<int> li{6, 2, 3, 4, 5, 1};
    {
        auto i = ranges::find(li, 6);
        if(i != ranges::end(li))
        {
            cout << "*i: " << *i;
        }
    }

    cout << "\nfwd_list: ";

    eastl::slist<int> fl{6, 2, 3, 4, 5, 1};
    {
        auto i = ranges::find(fl, 4);
        if(i != ranges::end(fl))
        {
            cout << "*i: " << *i;
        }
    }
    cout << "\ndeque:    ";

    eastl::deque<int> d{6, 2, 3, 4, 5, 1};
    {
        auto i = ranges::find(d, 6);
        if(i != ranges::end(d))
        {
            cout << "*i: " << *i;
        }
    }
    cout << '\n';
}
///[find]
