// Range v3 library
//
//  Copyright Filip Matner 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/ranges/view/for_each.hpp>
#include <EASTL/ranges/view/move.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

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


using namespace ranges;

int main()
{
    eastl::vector<eastl::unique_ptr<int>> d;
    d.emplace_back(eastl::unique_ptr<int>(new int(1)));
    d.emplace_back(eastl::unique_ptr<int>(new int(5)));
    d.emplace_back(eastl::unique_ptr<int>(new int(4)));

    auto rng = d | views::move | views::for_each([](eastl::unique_ptr<int> ptr)
    {
        return yield(*ptr);
    });

    check_equal(rng, {1, 5, 4});

    return ::test_result();
}
