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

#include <EASTL/functional.h>
#include <EASTL/iterator.h>
#include <EARanges/core.hpp>
#include <EARanges/utility/copy.hpp>
#include <EARanges/view/counted.hpp>
#include <EARanges/view/partial_sum.hpp>
#include <EARanges/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

void * __cdecl operator new[](size_t size, const char * name, int flags,
                              unsigned debugFlags, const char * file, int line)
{
    return new uint8_t[size];
}

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    {
        auto rng = rgi | views::partial_sum;
        has_type<int &>(*begin(rgi));
        has_type<int>(*begin(rng));
        CPP_assert(view_<decltype(rng)>);
        CPP_assert(sized_range<decltype(rng)>);
        CPP_assert(forward_range<decltype(rng)>);
        CPP_assert(!bidirectional_range<decltype(rng)>);
        ::check_equal(rng, {1, 3, 6, 10, 15, 21, 28, 36, 45, 55});

        auto it = begin(rng);
        CHECK(*it == 1);
        auto it2 = next(it);
        CHECK(*it == 1);
        CHECK(*it2 == 3);
        it2 = it;
        CHECK(*it2 == 1);
        ++it2;
        CHECK(*it2 == 3);
    }

    {
        // Test partial_sum with a mutable lambda
        int cnt = 0;
        auto mutable_rng = views::partial_sum(rgi, [cnt](int i, int j) mutable { return i + j + cnt++;});
        ::check_equal(mutable_rng, {1, 3, 7, 13, 21, 31, 43, 57, 73, 91});
        CHECK(cnt == 0);
        CPP_assert(view_<decltype(mutable_rng)>);
        CPP_assert(!view_<decltype(mutable_rng) const>);
    }

    {
        auto rng = debug_input_view<int const>{rgi} | views::partial_sum;
        ::check_equal(rng, {1, 3, 6, 10, 15, 21, 28, 36, 45, 55});
    }

    {
        static int const some_ints[] = {0,1,2,3,4};
        auto t1 = ranges::views::partial_sum(some_ints);
        auto t2 = some_ints | ranges::views::partial_sum;
        ::check_equal(t1, t2);
    }

    return test_result();
}
