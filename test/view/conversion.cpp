// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/utility.h> // for std::swap on C++14
#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/list.h>
#include <cstdio>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/any_view.hpp>
#include <EARanges/view/concat.hpp>
#include <EARanges/view/drop.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/repeat.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/zip.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int __cdecl EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments)
{
    return vsnprintf(pDestination, n, pFormat, arguments);
}

int main()
{
    using namespace ranges;

    // 1-d vector

    auto v = views::ints | views::take(10) | to<eastl::vector>();
    ::check_equal(v, {0,1,2,3,4,5,6,7,8,9});

    v = views::iota(10) | views::take(10) | views::reverse | to<eastl::vector>();
    ::check_equal(v, {19,18,17,16,15,14,13,12,11,10});

    // 1-d list

    auto l = views::ints | views::take(10) | to<eastl::list>();
    ::check_equal(l, {0,1,2,3,4,5,6,7,8,9});

    l = views::iota(10) | views::take(10) | views::reverse | to<eastl::list>();
    ::check_equal(l, {19,18,17,16,15,14,13,12,11,10});

    // 2-d vector

    auto vv = views::repeat_n(views::ints(0, 8), 10) | to<eastl::vector<eastl::vector<int>>>();
    ::check_equal(vv, eastl::vector<eastl::vector<int>>(10, {0,1,2,3,4,5,6,7}));

    // issue #556

    {
        eastl::string s{"abc"};
        any_view<any_view<char, category::random_access>, category::random_access> v1 = views::single(s | views::drop(1));
        any_view<any_view<char, category::random_access>, category::random_access> v2 = views::single(s | views::drop(2));
        auto v3 = views::concat(v1, v2);
        //TODO:30a) Conversion to eastl::strings are finicky
        //auto owner1 = v3 | to<eastl::vector<eastl::vector<char>>>();
        //auto owner2 = v3 | to<eastl::vector<eastl::string>>();
        //
        //::check_equal(owner1, eastl::vector<eastl::vector<char>>{{'b', 'c'}, {'c'}});
        //::check_equal(owner2, eastl::vector<eastl::string>{{"bc"}, {"c"}});
    }

    // map
    //TODO:30b) same here, especially with stringstream floating around
    auto to_string = [](int i){ return eastl::to_string(i); };
    //auto m = views::zip(views::ints, views::ints | views::transform(to_string)) | views::take(5) | to<eastl::map<int, eastl::string>>();
    using P = eastl::pair<int const, eastl::string>;
    //::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});
    
    // Another way to say the same thing, but with a range comprehension:
    auto m = views::for_each(views::ints(0,5), [&](int i)
        {
            return yield(eastl::make_pair(i, to_string(i)));
        }) | to<eastl::map<int, eastl::string>>();
    ::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});

    // set

    CPP_assert(range<eastl::set<int>>);
    CPP_assert(!view_<eastl::set<int>>);
    auto s = views::ints | views::take(10) | to<eastl::set<int>>();
    ::check_equal(s, {0,1,2,3,4,5,6,7,8,9});

    static_assert(!view_<std::initializer_list<int>>, "");

    return ::test_result();
}
