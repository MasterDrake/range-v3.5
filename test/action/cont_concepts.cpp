// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/action/concepts.hpp>
#include <EASTL/ranges/view/ref.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"



int main()
{
    using namespace ranges;

    int rgi[6];
    CPP_assert(range<decltype(rgi)>);
    CPP_assert(!semi_container<decltype(rgi)>);

    eastl::array<int, 6> a;
    CPP_assert(semi_container<decltype(a)>);
    CPP_assert(!container<decltype(a)>);

    eastl::vector<int> v;
    CPP_assert(container<decltype(v)>);

    eastl::vector<eastl::unique_ptr<int>> v2;
    CPP_assert(container<decltype(v2)>);

    CPP_assert(lvalue_container_like<decltype((v2))>);
    CPP_assert(!lvalue_container_like<decltype(eastl::move(v2))>);

    CPP_assert(lvalue_container_like<decltype(views::ref(v2))>);

    return ::test_result();
}
