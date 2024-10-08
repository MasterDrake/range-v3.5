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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EARanges/core.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EARanges/view/unbounded.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

void test()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr auto s = size(ia);
    int ib[s] = {0, 1, 2, 5, 4, 5};
    
    CHECK(equal(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)));
    CHECK(equal(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia + s)));
    CHECK(equal(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s)));
    CHECK(!equal(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  InputIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s)));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ib),
                  RandomAccessIterator<const int*>(ib+s)));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s)));
    CHECK(!equal(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia + s - 1)));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s-1)));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia + s - 1)));
}

void test_rng()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr auto s = size(ia);
    int ib[s] = {0, 1, 2, 5, 4, 5};
    CHECK(equal(make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s))));
    CHECK(equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia + s))));
    CHECK(equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s))));
    CHECK(!equal(make_subrange(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(InputIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s))));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ib),
                  RandomAccessIterator<const int*>(ib+s))));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s))));
    CHECK(!equal(make_subrange(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia + s - 1))));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s-1))));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia + s - 1))));
}

int comparison_count = 0;

template<typename T>
bool counting_equals(const T &a, const T &b)
{
    ++comparison_count;
    return a == b;
}

void test_pred()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr auto s = size(ia);
    int ib[s] = {0, 1, 2, 5, 4, 5};
    CHECK(equal(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s),
                 eastl::equal_to<int>()));
    CHECK(equal(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s),
                 eastl::equal_to<int>()));
    CHECK(equal(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s),
                 eastl::equal_to<int>()));

    comparison_count = 0;
    CHECK(!equal(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s - 1),
                 counting_equals<int>));
    CHECK(comparison_count > 0);
    comparison_count = 0;
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s-1),
                 counting_equals<int>));
    CHECK(comparison_count == 0);
    comparison_count = 0;
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s),
                 RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s - 1),
                 counting_equals<int>));
    CHECK(comparison_count > 0);
    CHECK(!equal(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  InputIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s),
                  eastl::equal_to<int>()));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ib),
                  RandomAccessIterator<const int*>(ib+s),
                  eastl::equal_to<int>()));
    CHECK(!equal(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s),
                  RandomAccessIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s),
                  eastl::equal_to<int>()));
}

void test_rng_pred()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr auto s = size(ia);
    int ib[s] = {0, 1, 2, 5, 4, 5};
    CHECK(equal(make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s)),
                 eastl::equal_to<int>()));
    CHECK(equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s)),
                 eastl::equal_to<int>()));
    CHECK(equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s)),
                 eastl::equal_to<int>()));

    comparison_count = 0;
    CHECK(!equal(make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(InputIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s - 1)),
                 counting_equals<int>));
    CHECK(comparison_count > 0);
    comparison_count = 0;
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 RandomAccessIterator<const int*>(ia+s-1)),
                 counting_equals<int>));
    CHECK(comparison_count == 0);
    comparison_count = 0;
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia+s)),
                 make_subrange(RandomAccessIterator<const int*>(ia),
                 Sentinel<const int*>(ia + s - 1)),
                 counting_equals<int>));
    CHECK(comparison_count > 0);
    CHECK(!equal(make_subrange(InputIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(InputIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s)),
                  eastl::equal_to<int>()));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  RandomAccessIterator<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ib),
                  RandomAccessIterator<const int*>(ib+s)),
                  eastl::equal_to<int>()));
    CHECK(!equal(make_subrange(RandomAccessIterator<const int*>(ia),
                  Sentinel<const int*>(ia+s)),
                  make_subrange(RandomAccessIterator<const int*>(ib),
                  Sentinel<const int*>(ib + s)),
                  eastl::equal_to<int>()));
}

int main()
{
    ::test();
    ::test_rng();
    ::test_pred();
    ::test_rng_pred();

    using IL = std::initializer_list<int>;
    int *p = nullptr;
    static_assert(eastl::is_same<bool, decltype(ranges::equal(IL{1, 2, 3, 4}, IL{1, 2, 3, 4}))>::value, "");
    static_assert(eastl::is_same<bool, decltype(ranges::equal(IL{1, 2, 3, 4}, ranges::views::unbounded(p)))>::value, "");

#if EARANGES_CXX_CONSTEXPR >= EARANGES_CXX_CONSTEXPR_14 && EARANGES_CONSTEXPR_INVOKE
    static_assert(ranges::equal(IL{1, 2, 3, 4}, IL{1, 2, 3, 4}), "");
    static_assert(!ranges::equal(IL{1, 2, 3, 4}, IL{1, 2, 3}), "");
    static_assert(!ranges::equal(IL{1, 2, 3, 4}, IL{1, 2, 4, 3}), "");
    static_assert(ranges::equal(IL{}, IL{}), "");
#endif

    return ::test_result();
}
