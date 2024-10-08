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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EASTL/utility.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/replace.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<typename Iter, typename Sent = Iter>
void test_iter()
{
    int ia[] = {0, 1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    Iter i = ranges::replace(Iter(ia), Sent(ia+sa), 2, 5);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 5);
    CHECK(ia[3] == 3);
    CHECK(ia[4] == 4);
    CHECK(base(i) == ia + sa);
}

template<typename Iter, typename Sent = Iter>
void test_rng()
{
    int ia[] = {0, 1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    auto rng = ranges::make_subrange(Iter(ia), Sent(ia+sa));
    Iter i = ranges::replace(rng, 2, 5);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 5);
    CHECK(ia[3] == 3);
    CHECK(ia[4] == 4);
    CHECK(base(i) == ia + sa);
}

constexpr bool test_constexpr()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4};
    constexpr auto sa = ranges::size(ia);
    auto r = ranges::replace(ia, 2, 42);
    STATIC_CHECK_RETURN(r == ia + sa);
    STATIC_CHECK_RETURN(ia[0] == 0);
    STATIC_CHECK_RETURN(ia[1] == 1);
    STATIC_CHECK_RETURN(ia[2] == 42);
    STATIC_CHECK_RETURN(ia[3] == 3);
    STATIC_CHECK_RETURN(ia[4] == 4);
    return true;
}

int main()
{
    test_iter<ForwardIterator<int*> >();
    test_iter<BidirectionalIterator<int*> >();
    test_iter<RandomAccessIterator<int*> >();
    test_iter<int*>();

    test_iter<ForwardIterator<int*>, Sentinel<int*> >();
    test_iter<BidirectionalIterator<int*>, Sentinel<int*> >();
    test_iter<RandomAccessIterator<int*>, Sentinel<int*> >();

    test_rng<ForwardIterator<int*> >();
    test_rng<BidirectionalIterator<int*> >();
    test_rng<RandomAccessIterator<int*> >();
    test_rng<int*>();

    test_rng<ForwardIterator<int*>, Sentinel<int*> >();
    test_rng<BidirectionalIterator<int*>, Sentinel<int*> >();
    test_rng<RandomAccessIterator<int*>, Sentinel<int*> >();

    // test projection
    {
        using P = eastl::pair<int,eastl::string>;
        P ia[] = {{0,"0"}, {1,"1"}, {2,"2"}, {3,"3"}, {4,"4"}};
        P *i = ranges::replace(ia, 2, eastl::make_pair(42,"42"), &eastl::pair<int,eastl::string>::first);
        CHECK(ia[0] == P{0,"0"});
        CHECK(ia[1] == P{1,"1"});
        CHECK(ia[2] == P{42,"42"});
        CHECK(ia[3] == P{3,"3"});
        CHECK(ia[4] == P{4,"4"});
        CHECK(i == ranges::end(ia));
    }

    // test rvalue ranges
    {
        using P = eastl::pair<int,eastl::string>;
        P ia[] = {{0,"0"}, {1,"1"}, {2,"2"}, {3,"3"}, {4,"4"}};
        auto i = ranges::replace(eastl::move(ia), 2, eastl::make_pair(42,"42"), &eastl::pair<int,eastl::string>::first);
#ifndef EARANGES_WORKAROUND_MSVC_573728
        CHECK(::is_dangling(i));
#endif // EARANGES_WORKAROUND_MSVC_573728
        CHECK(ia[0] == P{0,"0"});
        CHECK(ia[1] == P{1,"1"});
        CHECK(ia[2] == P{42,"42"});
        CHECK(ia[3] == P{3,"3"});
        CHECK(ia[4] == P{4,"4"});
    }

    {
        using P = eastl::pair<int,eastl::string>;
        eastl::vector<P> ia{{0,"0"}, {1,"1"}, {2,"2"}, {3,"3"}, {4,"4"}};
        auto i = ranges::replace(eastl::move(ia), 2, eastl::make_pair(42,"42"), &eastl::pair<int,eastl::string>::first);
        CHECK(::is_dangling(i));
        CHECK(ia[0] == P{0,"0"});
        CHECK(ia[1] == P{1,"1"});
        CHECK(ia[2] == P{42,"42"});
        CHECK(ia[3] == P{3,"3"});
        CHECK(ia[4] == P{4,"4"});
    }

    {
        STATIC_CHECK(test_constexpr());
    }

    return ::test_result();
}
