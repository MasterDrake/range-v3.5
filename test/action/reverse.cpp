// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/vector.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/repeat_n.hpp>
#include <EASTL/ranges/view/for_each.hpp>
#include <EASTL/ranges/action/reverse.hpp>
#include <EASTL/ranges/action/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;


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
    // [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...]
    auto v =
        views::for_each(views::ints(1,6), [](int i){
            return yield_from(views::repeat_n(i,i));
        }) | to<eastl::vector>();
    check_equal(v, {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5});

    v |= actions::unique | actions::reverse;
    check_equal(v, {5,4,3,2,1});

    return ::test_result();
}
