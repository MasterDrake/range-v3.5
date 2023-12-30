// Range v3 library
//
//  Copyright MikeGitb 2018-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EARanges/view/enumerate.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/indices.hpp>
#include <EARanges/view/transform.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/tuple.h>
#include <EASTL/iterator.h>


//TODO:32) BUGBUG there's a lot of problems here that all boil down to the operator != :O 
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

using eastl::begin;

template<class RangeT>
void test_enumerate_with(RangeT &&range)
{
    auto enumerated_range = ranges::views::enumerate(range);
    CPP_assert(ranges::borrowed_range<decltype(enumerated_range)>);

    std::size_t idx_ref = 0;
    auto it_ref = begin( range );

    for(auto it = enumerated_range.begin(); it != enumerated_range.end(); ++it)
    {
        const auto idx = eastl::get<0>(*it);
        const auto value = eastl::get<1>(*it);

        CHECK(idx == idx_ref++);
        CHECK(value == *it_ref++);
    }
}

int main()
{
    { // test array
        int const es[] = { 9,8,7,6,5,4,3,2,1,0 };
      //  test_enumerate_with(es);
    }

    { // test with vector of complex value type
        eastl::vector<eastl::list<int>> range{ {1, 2, 3}, { 3,5,6,7 }, { 10,5,6,1 }, { 1,2,3,4 } };
        const auto rcopy = range;

        //test_enumerate_with(range);

        // check that range hasn't accidentially been modified
        CHECK(rcopy == range);

        // check with empty range
        range.clear();
       // test_enumerate_with(range);
    }

    { // test with list
        eastl::list<int> range{ 9,8,7,6,5,4,3,2,1 };
       // test_enumerate_with(range);

        range.clear();
       // test_enumerate_with(range);
    }

    { // test with initializer_list
       // test_enumerate_with(std::initializer_list<int>{9, 8, 7, 6, 5, 4, 3, 2, 1});
    }

    {
        auto range = ranges::views::iota(0, 0);
       // test_enumerate_with(range);

        range = ranges::views::iota(-10000, 10000);
       // test_enumerate_with(range);
    }

    {
        auto range = ranges::views::iota((std::uintmax_t)0, (std::uintmax_t)0);
       // test_enumerate_with(range);

        auto range2 = ranges::views::iota((std::intmax_t) -10000, (std::intmax_t) 10000);
       // test_enumerate_with(range2);
    }

    // https://github.com/ericniebler/range-v3/issues/1141
    {
        using namespace ranges;
        auto x = views::indices( std::uintmax_t( 100 ) )
          | views::transform([](std::uintmax_t) { return "";})
          | views::enumerate;
        using X = decltype(x);
        CPP_assert(same_as<range_difference_t<X>, detail::diffmax_t>);
        CPP_assert(same_as<range_value_t<X>, eastl::pair<detail::diffmax_t, char const*>>);
    }

    return ::test_result();
}
