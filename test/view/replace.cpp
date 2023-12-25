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

#include <EASTL/string.h>
#include <sstream>
#include <EASTL/vector.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/istream.hpp>
#include <EASTL/ranges/functional/reference_wrapper.hpp>
#include <EASTL/ranges/utility/copy.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/replace.hpp>
#include <EASTL/ranges/view/common.hpp>
#include <EASTL/ranges/view/take.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

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
    using namespace ranges;

    eastl::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str.c_str()};

    {
        auto rng = istream<int>(sin) | views::replace(1, 42);
        CPP_assert(same_as<range_value_t<decltype((rng))>, int>);
        has_type<int const &>(*begin(rng));
        CPP_assert(view_<decltype(rng)>);
        CPP_assert(!sized_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
        CPP_assert(input_iterator<decltype(begin(rng))>);
        CPP_assert(!forward_iterator<decltype(begin(rng))>);

        auto tmp = rng | views::common;
        CPP_assert(same_as<range_value_t<decltype((tmp))>, int>);
        has_type<int const &>(*begin(tmp));
        CPP_assert(view_<decltype(tmp)>);
        CPP_assert(common_range<decltype(tmp)>);
        CPP_assert(!sized_range<decltype(tmp)>);
        CPP_assert(input_iterator<decltype(begin(tmp))>);
        CPP_assert(!forward_iterator<decltype(begin(tmp))>);
        eastl::vector<int> actual{begin(tmp), end(tmp)};
        ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    eastl::vector<int> rgi{1,2,3,4,5,6,7,8,9};

    {
        auto rng2 = rgi | views::replace(5, 42);
        CPP_assert(same_as<range_value_t<decltype((rng2))>, int>);
        has_type<int const &>(*begin(rng2));
        has_type<int const &>(iter_move(begin(rng2)));
        CPP_assert(view_<decltype(rng2)>);
        CPP_assert(sized_range<decltype(rng2)>);
        CPP_assert(common_range<decltype(rng2)>);
        CPP_assert(random_access_iterator<decltype(begin(rng2))>);
        ::check_equal(rng2, {1,2,3,4,42,6,7,8,9});
    }

    {
        int forty_two = 42;
        auto rng3 = rgi | views::replace(5, ref(forty_two));
        CPP_assert(same_as<range_value_t<decltype((rng3))>, int>);
        has_type<int &>(*begin(rng3));
        has_type<int const &>(iter_move(begin(rng3)));
        CPP_assert(view_<decltype(rng3)>);
        CPP_assert(sized_range<decltype(rng3)>);
        CPP_assert(common_range<decltype(rng3)>);
        CPP_assert(random_access_iterator<decltype(begin(rng3))>);
        ::check_equal(rng3, {1,2,3,4,42,6,7,8,9});
    }

    {
        auto rng4 = views::ints | views::replace(5,42) | views::take(10);
        CPP_assert(same_as<range_value_t<decltype((rng4))>, int>);
        has_type<int>(*begin(rng4));
        has_type<int>(iter_move(begin(rng4)));
        CPP_assert(view_<decltype(rng4)>);
        CPP_assert(sized_range<decltype(rng4)>);
        CPP_assert(!common_range<decltype(rng4)>);
        CPP_assert(random_access_iterator<decltype(begin(rng4))>);
        ::check_equal(rng4, {0,1,2,3,4,42,6,7,8,9});
    }

    {
        int const some_ints[] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9};
        auto rng = debug_input_view<int const>{some_ints} | views::replace(1, 42);
        ::check_equal(rng, {42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9});
    }

    return test_result();
}
