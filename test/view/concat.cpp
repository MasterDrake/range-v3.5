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

#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EARanges/core.hpp>
#include <EARanges/view/concat.hpp>
#include <EARanges/view/generate.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/remove_if.hpp>
#include <EARanges/view/take_while.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    eastl::vector<eastl::string> his_face{"this", "is", "his", "face"};
    eastl::vector<eastl::string> another_mess{"another", "fine", "mess"};
    auto joined = views::concat(his_face, another_mess);
    CPP_assert(view_<decltype(joined)>);
    CPP_assert(random_access_range<decltype(joined)>);
    static_assert(eastl::is_same<range_reference_t<decltype(joined)>, eastl::string &>::value, "");
    CHECK(joined.size() == 7u);
    CHECK((joined.end() - joined.begin()) == 7);
    ::check_equal(joined | views::reverse, {"mess", "fine", "another", "face", "his", "is", "this"});

    auto revjoin = joined | views::reverse;
    CHECK((revjoin.end() - revjoin.begin()) == 7);

    auto first = joined.begin();
    CHECK(*(first+0) == "this");
    CHECK(*(first+1) == "is");
    CHECK(*(first+2) == "his");
    CHECK(*(first+3) == "face");
    CHECK(*(first+4) == "another");
    CHECK(*(first+5) == "fine");
    CHECK(*(first+6) == "mess");

    CHECK(*(first) == "this");
    CHECK(*(first+=1) == "is");
    CHECK(*(first+=1) == "his");
    CHECK(*(first+=1) == "face");
    CHECK(*(first+=1) == "another");
    CHECK(*(first+=1) == "fine");
    CHECK(*(first+=1) == "mess");

    auto last = joined.end();
    CHECK(*(last-1) == "mess");
    CHECK(*(last-2) == "fine");
    CHECK(*(last-3) == "another");
    CHECK(*(last-4) == "face");
    CHECK(*(last-5) == "his");
    CHECK(*(last-6) == "is");
    CHECK(*(last-7) == "this");

    CHECK(*(last-=1) == "mess");
    CHECK(*(last-=1) == "fine");
    CHECK(*(last-=1) == "another");
    CHECK(*(last-=1) == "face");
    CHECK(*(last-=1) == "his");
    CHECK(*(last-=1) == "is");
    CHECK(*(last-=1) == "this");

    {
        const eastl::array<int, 3> a{{0, 1, 2}};
        const eastl::array<int, 2> b{{3, 4}};
        check_equal(views::concat(a, b), {0, 1, 2, 3, 4});

        auto odd = [](int i) { return i % 2 != 0; };
        auto even_filter = ranges::views::remove_if(odd);

        auto f_rng0 = a | even_filter;
        auto f_rng1 = b | even_filter;

        check_equal(views::concat(f_rng0, f_rng1), {0, 2, 4});
    }

    // Regression test for http://github.com/ericniebler/range-v3/issues/395.
    {
        int i = 0;
        auto rng = ranges::views::concat(ranges::views::generate([&]{ return i++; }))
            | ranges::views::take_while([](int j){ return j < 30; });
        CHECK(ranges::distance(ranges::begin(rng), ranges::end(rng)) == 30);
    }

    {
        int const rgi[] = {0,1,2,3};
        auto dv = [&]{ return debug_input_view<int const>{rgi}; };
        auto rng = views::concat(dv(), dv(), dv());
        ::check_equal(rng, {0,1,2,3,0,1,2,3,0,1,2,3});
    }

    return test_result();
}
