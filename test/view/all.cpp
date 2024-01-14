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

#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/all.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {1, 1, 1, 2, 3, 4, 4};
    eastl::vector<int> vi(begin(rgi), end(rgi));
    eastl::list<int> li(begin(rgi), end(rgi));

    ref_view<int[7]> x = views::all(rgi);
    ref_view<eastl::vector<int>> y = views::all(vi);
    ref_view<eastl::list<int>> z = views::all(li);

    CPP_assert(sized_range<decltype(x)> && view_<decltype(x)>);
    CPP_assert(sized_range<decltype(y)> && view_<decltype(y)>);
    CPP_assert(sized_range<decltype(z)> && view_<decltype(z)>);

    x = views::all(x);
    y = views::all(y);
    z = views::all(z);

    CHECK(x.size() == 7u);
    CHECK(y.size() == 7u);
    CHECK(z.size() == 7u);

    {
        auto v = views::all(debug_input_view<int const>{rgi});
        CHECK(v.size() == size(rgi));
        CHECK(v.data_->first_ == rgi);
        auto v2 = views::all(views::all(views::all(eastl::move(v))));
        CPP_assert(same_as<decltype(v), decltype(v2)>);
        CHECK(v2.size() == size(rgi));
        CHECK(v2.data_->first_ == rgi);
    }

    return test_result();
}
