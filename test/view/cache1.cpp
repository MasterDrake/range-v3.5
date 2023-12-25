/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014.
//  Copyright Casey Carter 2017.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <EASTL/vector.h>
#include <EASTL/ranges/view/cache1.hpp>
#include <EASTL/ranges/view/transform.hpp>
#include <EASTL/ranges/view/c_str.hpp>
#include <EASTL/ranges/view/move.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
using namespace ranges;

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

int main()
{
    {
        int count = 0;
        eastl::vector<int> v{1,2,3};
        auto rng = v | views::transform([&count](int i){ ++count; return i;})
            | views::cache1;
        using Rng = decltype(rng);
        CPP_assert(!range<Rng const>);
        CPP_assert(input_range<Rng>);
        CPP_assert(!forward_range<Rng>);
        CPP_assert(common_range<Rng>);
        CPP_assert(view_<Rng>);
        CPP_assert(sized_range<Rng>);
        CPP_assert(sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>);
        CPP_assert(same_as<range_value_t<Rng>, int>);
        CPP_assert(same_as<range_reference_t<Rng>, int &&>);
        CPP_assert(same_as<range_rvalue_reference_t<Rng>, int &&>);
        CHECK(count == 0);
        auto it = ranges::begin(rng);
        CHECK(count == 0);
        auto last = ranges::end(rng);
        CHECK(it != last);
        CHECK(count == 0);
        CHECK(*it == 1);
        CHECK(count == 1);
        CHECK(*it == 1);
        CHECK(count == 1);
        ++it;
        CHECK(it != last);
        CHECK(count == 1);
        CHECK(*it == 2);
        CHECK(count == 2);
        CHECK(*it == 2);
        CHECK(count == 2);
        ++it;
        CHECK(it != last);
        CHECK(count == 2);
        CHECK(*it == 3);
        CHECK(count == 3);
        CHECK(*it == 3);
        CHECK(count == 3);
        ++it;
        CHECK(count == 3);
        CHECK(it == last);
    }

    {
        int count = 0;
        char const * hi = "hi";
        auto rng = views::c_str(hi)
            | views::transform([&count](char ch){ ++count; return ch;})
            | views::cache1;
        using Rng = decltype(rng);
        CPP_assert(!range<Rng const>);
        CPP_assert(input_range<Rng>);
        CPP_assert(!forward_range<Rng>);
        CPP_assert(!common_range<Rng>);
        CPP_assert(view_<Rng>);
        CPP_assert(!sized_range<Rng>);
        CPP_assert(!sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>);
        CPP_assert(same_as<range_value_t<Rng>, char>);
        CPP_assert(same_as<range_reference_t<Rng>, char &&>);
        CPP_assert(same_as<range_rvalue_reference_t<Rng>, char &&>);
        CHECK(count == 0);
        auto it = ranges::begin(rng);
        CHECK(count == 0);
        auto last = ranges::end(rng);
        CHECK(it != last);
        CHECK(count == 0);
        CHECK(*it == 'h');
        CHECK(count == 1);
        CHECK(*it == 'h');
        CHECK(count == 1);
        ++it;
        CHECK(it != last);
        CHECK(count == 1);
        CHECK(*it == 'i');
        CHECK(count == 2);
        CHECK(*it == 'i');
        CHECK(count == 2);
        ++it;
        CHECK(count == 2);
        CHECK(it == last);
    }

    {
        int count = 0;
        MoveOnlyString rg[] = {"hello", "world"};
        auto rng = rg
            | views::move
            | views::transform([&count](auto s){ ++count; RANGES_ENSURE(s != ""); return s;})
            | views::cache1;
        using Rng = decltype(rng);
        CPP_assert(!range<Rng const>);
        CPP_assert(input_range<Rng>);
        CPP_assert(!forward_range<Rng>);
        CPP_assert(common_range<Rng>);
        CPP_assert(view_<Rng>);
        CPP_assert(sized_range<Rng>);
        CPP_assert(sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>);
        CPP_assert(same_as<range_value_t<Rng>, MoveOnlyString>);
        CPP_assert(same_as<range_reference_t<Rng>, MoveOnlyString &&>);
        CPP_assert(same_as<range_rvalue_reference_t<Rng>, MoveOnlyString &&>);
        CHECK(count == 0);
        auto it = ranges::begin(rng);
        CHECK(count == 0);
        auto last = ranges::end(rng);
        CHECK(it != last);
        CHECK(count == 0);
        CHECK((*it == "hello"));
        CHECK(count == 1);
        CHECK((*it == "hello"));
        CHECK(count == 1);
        ++it;
        CHECK(it != last);
        CHECK(count == 1);
        CHECK((*it == "world"));
        CHECK(count == 2);
        CHECK((*it == "world"));
        CHECK(count == 2);
        ++it;
        CHECK(count == 2);
        CHECK(it == last);
    }

    return test_result();
}
