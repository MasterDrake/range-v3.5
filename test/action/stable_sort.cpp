// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <EASTL/random.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/stride.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/algorithm/shuffle.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/is_sorted.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/action/shuffle.hpp>
#include <EARanges/action/stable_sort.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

#if !defined(__clang__) || !defined(_MSVC_STL_VERSION) // Avoid #890
void test_bug632()
{
    const eastl::vector<double> scores = { 3.0, 1.0, 2.0 };
    eastl::vector<int> indices = { 0, 1, 2 };

    indices |= ranges::actions::stable_sort(ranges::less{}, [&] (const int &x) { return scores[ (eastl::size_t)x ]; } );

    ::check_equal( indices, {1, 2, 0} );
}

int main()
{
    using namespace ranges;
    eastl::default_random_engine gen;

    auto v = views::ints(0,100) | to<eastl::vector>();
    v |= actions::shuffle(gen);
    CHECK(!is_sorted(v));

    auto v2 = v | copy | actions::stable_sort;
    CHECK(size(v2) == size(v));
    CHECK(is_sorted(v2));
    CHECK(!is_sorted(v));
    CPP_assert(same_as<decltype(v), decltype(v2)>);

    v |= actions::stable_sort;
    CHECK(is_sorted(v));

    v |= actions::shuffle(gen);
    CHECK(!is_sorted(v));

    v = v | move | actions::stable_sort(eastl::less<int>());
    CHECK(is_sorted(v));
    CHECK(equal(v, v2));

    // Container algorithms can also be called directly
    // in which case they take and return by reference
    shuffle(v, gen);
    CHECK(!is_sorted(v));
    auto & v3 = actions::stable_sort(v);
    CHECK(is_sorted(v));
    CHECK(&v3 == &v);

    auto r = views::ref(v);
    r |= actions::stable_sort;

    // Can pipe a view to a "container" algorithm.
    actions::stable_sort(v, eastl::greater<int>());
    v | views::stride(2) | actions::stable_sort;
    check_equal(views::take(v, 10), {1,98,3,96,5,94,7,92,9,90});

    test_bug632();

    return ::test_result();
}
#else // Avoid #890
int main() {}
#endif // Avoid #890
