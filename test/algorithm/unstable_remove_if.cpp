// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/ranges/algorithm/equal.hpp>
#include <EASTL/ranges/algorithm/unstable_remove_if.hpp>
#include <EASTL/ranges/view/subrange.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"

constexpr bool is_even(int i)
{
    return i % 2 == 0;
}

//constexpr bool test_constexpr()
//{
//    using IL = std::initializer_list<int>;
//
//    test::array<int, 5> arr{{1, 2, 3, 4, 5}};
//    const auto it = ranges::unstable_remove_if(arr, is_even);
//    STATIC_CHECK_RETURN(it == arr.begin() + 3);
//    STATIC_CHECK_RETURN(ranges::equal(ranges::make_subrange(arr.begin(), it), IL{1, 5, 3}));
//
//    return true;
//}

int main()
{
    //todo:constexpr vs eastl::addressof
  //  STATIC_CHECK(test_constexpr());

    return test_result();
}
