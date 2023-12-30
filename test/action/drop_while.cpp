// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/action/drop_while.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

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
    using namespace ranges;
    using namespace std::placeholders;

    auto v = views::ints(1,21) | to<eastl::vector>();
    auto & v2 = actions::drop_while(v, std::bind(eastl::less<int>(), _1, 4));
    CHECK(&v2 == &v);
    CHECK(v.size() == 17u);
    CHECK(v[0] == 4);

    v = eastl::move(v) | actions::drop_while([](int i){return i < 7;});
    CHECK(v.size() == 14u);
    CHECK(v[0] == 7);

    v |= actions::drop_while([](int i){return i < 10;});
    CHECK(v.size() == 11u);
    CHECK(v[0] == 10);

    v |= actions::drop_while([](int){return true;});
    CHECK(v.size() == 0u);

    return ::test_result();
}
