// Range v3 library
//
//  Copyright Filip Matzner 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/list.h>
#include <EASTL/memory.h>
#include <EASTL/tuple.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/for_each.hpp>
#include <EARanges/view/cycle.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/join.hpp>
#include <EARanges/view/remove_if.hpp>
#include <EARanges/view/repeat.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/experimental/view/shared.hpp>
#include <EARanges/view/take.hpp>
#include "../../simple_test.hpp"
#include "../../test_utils.hpp"

using namespace ranges;
static size_t counter = 0;
static size_t totalSize = 0;
void * __cdecl operator new[](size_t size, const char * name, int flags,
                              unsigned debugFlags, const char * file, int line)
{
    std::cout << "Allocation #" << counter++ << " Size: " << size <<  "(" << (totalSize += size) << ")" << " in " << file << ":" << line << ".\n";

    return new uint8_t[size];
}

void * __cdecl operator new[](size_t size, size_t alignement, size_t offset,
                              const char * name, int flags, unsigned debugFlags,
                              const char * file, int line)
{
    
    std::cout << "Allocation #" << counter++ << " Size: " << size << "(" << alignement << ")" << " in " << file << ":" << line << ".\n";
    return new uint8_t[size];
}

template<typename T>
void check_shared_contents()
{
    // build two instances sharing the same range
    experimental::shared_view<T> view1 = experimental::views::shared(T{1, 1, 1, 2, 3, 4, 4});
    experimental::shared_view<T> view2 = view1;

    // check the length of the views
    CHECK(view1.size() == 7u);
    CHECK(view2.size() == 7u);

    // check the stored numbers
    auto check_values = [](experimental::shared_view<T> & rng)
    {
      ::check_equal(views::cycle(rng) | views::take(10), {1, 1, 1, 2, 3, 4, 4, 1, 1, 1});
      ::check_equal(views::all(rng) | views::take(5), {1, 1, 1, 2, 3});
      ::check_equal(rng | views::take(5), {1, 1, 1, 2, 3});
      ::check_equal(rng, {1, 1, 1, 2, 3, 4, 4});
    };
    check_values(view1);
    check_values(view2);
    //TODO:5)Apparently shared_view needs a l-value here for the operator++()... Do we care?
    // check that changes are shared
    *(++begin(view1)) = 7;
    CHECK(*(++begin(view2)) == 7);
    *begin(view2) = 3;
    CHECK(*begin(view1) == 3);
}

int main()
{
    // check shared random access range
    check_shared_contents<eastl::vector<int>>();
    // check shared bidirectional range
    check_shared_contents<eastl::list<int>>();

    {
        // check the piped construction from an rvalue
        eastl::vector<int> base_vec = {1, 2, 2, 8, 2, 7};
        auto vec_view = eastl::move(base_vec) | experimental::views::shared;
        CHECK(vec_view.size() == 6u);
        ::check_equal(vec_view, {1, 2, 2, 8, 2, 7});
    }

    {
        // test bidirectional range
        auto list_view = eastl::list<int>{1, 2, 3} | experimental::views::shared;

        CHECK(list_view.size() == 3u);
        has_type<int &>(*begin(list_view));
        CPP_assert(sized_range<decltype(list_view)>);
        CPP_assert(common_range<decltype(list_view)>);
        CPP_assert(bidirectional_range<decltype(list_view)>);
        CPP_assert(!random_access_range<decltype(list_view)>);

        // test bidirectional range iterator
        CHECK(*begin(list_view) == 1);
        CHECK(*prev(end(list_view)) == 3);
    }

    {
        // test random access range
        auto vec_view = eastl::vector<int>{1, 2, 3} | experimental::views::shared;

        CHECK(vec_view.size() == 3u);
        has_type<int &>(*begin(vec_view));
        CPP_assert(sized_range<decltype(vec_view)>);
        CPP_assert(common_range<decltype(vec_view)>);
        CPP_assert(random_access_range<decltype(vec_view)>);
        CHECK(vec_view[0] == 1);
        CHECK(vec_view[1] == 2);
        CHECK(vec_view[2] == 3);
    }

    {
        // check temporary value in views::transform
        auto f = [](unsigned a){ return eastl::vector<unsigned>(a, a); };

        auto vec_view =
            views::iota(1u)
          | views::transform(f)
          | views::transform(experimental::views::shared)
          | views::join
          | views::take(10);

        ::check_equal(vec_view, {1u, 2u, 2u, 3u, 3u, 3u, 4u, 4u, 4u, 4u});
    }

    {
        // check temporary value in views::for_each
        eastl::vector<int> base_vec{1, 2, 3};
        auto vec_view =
            views::repeat(base_vec)
          | views::for_each([](eastl::vector<int> tmp)
            {
                return yield_from(eastl::move(tmp) | experimental::views::shared | views::reverse);
            })
          | views::take(7);
        ::check_equal(vec_view, {3, 2, 1, 3, 2, 1, 3});
    }

    {
        // check temporary value in views::for_each without the yield_from
        eastl::vector<int> base_vec{1, 2, 3};
        auto vec_view =
            views::repeat(base_vec)
          | views::for_each([](eastl::vector<int> tmp)
            {
                return eastl::move(tmp) | experimental::views::shared | views::reverse;
            })
          | views::take(7);
        ::check_equal(vec_view, {3, 2, 1, 3, 2, 1, 3});
    }

    return test_result();
}
