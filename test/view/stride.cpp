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
#include <sstream>
#include <EARanges/core.hpp>
#include <EARanges/view/istream.hpp>
#include <EARanges/view/move.hpp>
#include <EARanges/view/partial_sum.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/stride.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/insert_iterators.hpp>
#include <EARanges/iterator/stream_iterators.hpp>
#include <EARanges/numeric.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

// https://github.com/ericniebler/range-v3/issues/1291
void bug_1291()
{
    eastl::vector<int> vec;

    auto tx = vec | ranges::views::stride( 2 ) | ranges::views::partial_sum;
    ranges::accumulate( tx, 0 );
}

int main()
{
    using namespace ranges;

    auto const v = []
    {
        eastl::vector<int> vec(50);
        iota(vec, 0);
        return vec;
    }();
    {
        auto rng = v | views::stride(3);
        using R = decltype(rng);
        CPP_assert(random_access_range<R> && view_<R>);
        CPP_assert(!contiguous_range<R>);
        CPP_assert(common_range<R>);
        CPP_assert(sized_range<R>);
        CPP_assert(range<R const>);
        ::check_equal(rng | views::reverse, {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    {
        std::stringstream str;
        copy(v, ostream_iterator<int>{str, " "});
        auto rng = istream<int>(str) | views::stride(3);
        using R = decltype(rng);
        CPP_assert(input_range<R> && view_<R>);
        CPP_assert(!forward_range<R>);
        CPP_assert(!common_range<R>);
        CPP_assert(!sized_range<R>);
        CPP_assert(!range<R const>);
        check_equal(rng, {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
    }

    {
        eastl::list<int> li;
        copy(v, back_inserter(li));
        auto rng = li | views::stride(3);
        using R = decltype(rng);
        CPP_assert(bidirectional_range<R> && view_<R>);
        CPP_assert(!random_access_range<R>);
        CPP_assert(common_range<R>);
        CPP_assert(sized_range<R>);
        CPP_assert(range<R const>);
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | views::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});

        for(int i : rng | views::reverse)
            std::cout << i << ' ';
        std::cout << '\n';
    }

    {
        auto x2 = v | views::stride(3);
        CHECK(ranges::distance(x2) == 17);

        auto it0 = x2.begin();
        auto it1 = eastl::next(it0, 10);
        CHECK((it1 - it0) == 10);
        CHECK((it0 - it1) == -10);
        CHECK((it0 - it0) == 0);
        CHECK((it1 - it1) == 0);
    }

    {
        const auto n = 4;
        auto rng = v | views::move | views::stride(2);
        CHECK((next(begin(rng), n) - begin(rng)) == n);
    }

    {
        // Regression test #368
        int n = 42;
        (void)ranges::views::stride(n);
    }

    {
        int const some_ints[] = {0,1,2,3,4,5,6,7};
        auto rng = debug_input_view<int const>{some_ints} | views::stride(2);
        using R = decltype(rng);
        CPP_assert(input_range<R> && view_<R>);
        CPP_assert(!forward_range<R>);
        CPP_assert(!common_range<R>);
        CPP_assert(sized_range<R>);
        CPP_assert(!range<R const>);
        ::check_equal(rng, {0,2,4,6});
    }

    {
        eastl::list<int> li;
        copy(v, back_inserter(li));
        subrange<eastl::list<int>::const_iterator> tmp{li.begin(), li.end()};
        auto rng = tmp | views::stride(3);
        using R = decltype(rng);
        CPP_assert(bidirectional_range<R> && view_<R>);
        CPP_assert(!random_access_range<R>);
        CPP_assert(!common_range<R>);
        CPP_assert(!sized_range<R>);
        CPP_assert(!range<R const>);
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | views::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    {
        eastl::list<int> li;
        copy(v, back_inserter(li));
        using CLI = eastl::list<int>::const_iterator;
        subrange<CLI, CLI, subrange_kind::sized> tmp{li};
        auto rng = tmp | views::stride(3);
        using R = decltype(rng);
        CPP_assert(bidirectional_range<R> && view_<R>);
        CPP_assert(!random_access_range<R>);
        CPP_assert(common_range<R>);
        CPP_assert(sized_range<R>);
        CPP_assert(range<R const>);
        CHECK((*--rng.end()) == 48);
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | views::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    // https://github.com/ericniebler/range-v3/issues/901
    {
        auto r = views::iota( 0, 12 );

        // Evenly divisible stride:
        auto strided1 = r | views::stride(3);
        ::check_equal(strided1, {0, 3, 6, 9});
        CHECK(strided1.size() == 4u);
        CHECK(strided1.front() == 0);
        CHECK(strided1[0] == 0);
        CHECK(strided1.back() == 9);
        CHECK(strided1[3] == 9);
        CHECK(strided1[(int)strided1.size() - 1] == 9);

        // Not evenly divisible stride:
        auto strided2 = r | views::stride(5);
        ::check_equal(strided2, {0, 5, 10});
        CHECK(strided2.size() == 3u);
        CHECK(strided2.front() == 0);
        CHECK(strided2[0] == 0);
        CHECK(strided2.back() == 10);
        CHECK(strided2[2] == 10);
        CHECK(strided2[(int)strided2.size() - 1] == 10);
    }

    return ::test_result();
}
