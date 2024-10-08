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

#include <EASTL/map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/map.hpp>
#include <EARanges/view/zip.hpp>
#include <EARanges/utility/copy.hpp>
#include <EARanges/algorithm/find.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    eastl::map<eastl::string, int> m =
    {
        {"this", 0},
        {"that", 1},
        {"other", 2}
    };
    auto keys = m | views::keys;
    has_type<eastl::string const &>(*begin(keys));
    CPP_assert(view_<decltype(keys)>);
    CPP_assert(sized_range<decltype(keys)>);
    CPP_assert(common_range<decltype(keys)>);
    CPP_assert(bidirectional_iterator<decltype(begin(keys))>);
    CHECK(&*begin(keys) == &m.begin()->first);
    ::check_equal(keys, {"other", "that", "this"});

    auto values = m | views::values;
    has_type<int &>(*begin(values));
    CPP_assert(sized_range<decltype(values)>);
    CPP_assert(common_range<decltype(values)>);
    CPP_assert(bidirectional_iterator<decltype(begin(values))>);
    CHECK(&*begin(values) == &m.begin()->second);
    ::check_equal(values, {2, 1, 0});

    {
        // regression test for #526
        auto f = detail::get_first{};
        CPP_assert(same_as<int, decltype(f(eastl::declval<eastl::pair<int,int>>()))>);
        CPP_assert(same_as<int&, decltype(f(eastl::declval<eastl::pair<int,int>&>()))>);
        CPP_assert(same_as<int&, decltype(f(eastl::declval<eastl::pair<int&,int&>>()))>);
        CPP_assert(same_as<int&, decltype(f(eastl::declval<eastl::pair<int&,int&>&>()))>);
        CPP_assert(same_as<int, decltype(f(eastl::declval<eastl::pair<int&&,int&&>>()))>);
        CPP_assert(same_as<int&, decltype(f(eastl::declval<eastl::pair<int&&,int&&>&>()))>);

        eastl::vector<int> xs = {42, 100, -1234};
        auto exs = views::zip(views::ints, xs);
       // Could be testing related and that's it?
       ::check_equal(views::keys(exs), {0, 1, 2});
    }

    {
        eastl::pair<int, int> const rgp[] = {{0, 2}, {1, 1}, {2, 0}};
        auto key_range = debug_input_view<eastl::pair<int, int> const>{rgp} | views::keys;
        check_equal(key_range, {0,1,2});
        auto value_range = debug_input_view<eastl::pair<int, int> const>{rgp} | views::values;
        check_equal(value_range, {2,1,0});
    }

    {
        auto it = find(m | views::keys, "other");
        CHECK(it.base()->second == 2);
        auto it2 = find(m | views::values, 1);
        CHECK(it2.base()->first == "that");
    }

    return test_result();
}
