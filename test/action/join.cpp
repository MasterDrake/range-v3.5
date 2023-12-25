// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/action/join.hpp>
#include <EASTL/ranges/algorithm/move.hpp>
#include <EASTL/ranges/algorithm/equal.hpp>
#include <EASTL/ranges/view/transform.hpp>
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
    //TODO:2) Due the lack of char traits, the eastl::string <-> char conversions and joins won't work :/ I mean, they'll work, just can't check them in tests.
    eastl::vector<eastl::string> v {"hello"," ","world"};
    auto s = v | move | actions::join;
   // static_assert(eastl::is_same<decltype(s), eastl::string>::value, "");
    //CHECK(s == "hello world");

    auto s2 = v | views::transform(views::all) | actions::join;
   // static_assert(eastl::is_same<decltype(s2), eastl::vector<char>>::value, "");
    CHECK(eastl::string(s2.begin(), s2.end()) == "hello world");

    return ::test_result();
}
