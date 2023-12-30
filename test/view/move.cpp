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

#include <cstring>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/move.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/utility/copy.hpp>
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
    static const char * const data[] = {"'allo", "'allo", "???"};
    eastl::vector<MoveOnlyString> vs(begin(data), end(data));
    auto x = vs | views::move;

    {
        CPP_assert(common_range<decltype(x)>);
        CPP_assert(sized_range<decltype(x)>);
        CPP_assert(view_<decltype(x)>);
        CPP_assert(common_range<decltype(x)>);
        CPP_assert(sized_range<decltype(x)>);
        CPP_assert(random_access_iterator<decltype(x.begin())>);
        using I = decltype(x.begin());
        CPP_assert(same_as<iterator_tag_of<I>, eastl::random_access_iterator_tag>);
        CPP_assert(same_as<typename eastl::iterator_traits<I>::iterator_category, eastl::random_access_iterator_tag>);

        CHECK(bool(*x.begin() == "'allo"));
    }

    {
        eastl::vector<MoveOnlyString> vs2(x.begin(), x.end());
        static_assert(eastl::is_same<MoveOnlyString&&, decltype(*x.begin())>::value, "");
        ::check_equal(vs2, {"'allo", "'allo", "???"});
        ::check_equal(vs, {"", "", ""});
    }

    {
        MoveOnlyString rgs[] = {"can", "you", "hear", "me", "now?"};
        auto rng = debug_input_view<MoveOnlyString>{rgs} | views::move;
        MoveOnlyString target[sizeof(rgs) / sizeof(rgs[0])];
        copy(rng, target);
        ::check_equal(rgs, {"", "", "", "", ""});
        ::check_equal(target, {"can", "you", "hear", "me", "now?"});
    }

    return test_result();
}
