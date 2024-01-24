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
#include <EARanges/core.hpp>
#include <EARanges/action/join.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/view/transform.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    eastl::vector<eastl::string> v {"hello"," ","world"};
    auto s = v | move | actions::join;
    static_assert(ranges::container<eastl::string>, "");
    static_assert(eastl::is_same<decltype(s), eastl::string>::value, "");
    CHECK(s == "hello world");
    
    auto s2 = v | views::transform(views::all) | actions::join;
    static_assert(eastl::is_same<decltype(s2), eastl::vector<char>>::value, "");
    CHECK(eastl::string(s2.begin(), s2.end()) == "hello world");

    return ::test_result();
}