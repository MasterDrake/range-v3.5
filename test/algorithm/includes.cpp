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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EASTL/functional.h>

#include <EARanges/algorithm/set_algorithm.hpp>
#include <EARanges/core.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
static size_t counter = 0;
void * __cdecl operator new[](size_t size, size_t alignement, size_t offset,
                              const char * name, int flags, unsigned debugFlags,
                              const char * file, int line)
{
    std::cout << "Allocation:#" << counter++ << " Size: " << size << " in " << file << ": " << line << ".\n";
    return new uint8_t[size];
}

namespace
{
    auto const true_ = [](bool b) { CHECK(b); };
    auto const false_ = [](bool b) { CHECK(!b); };

    template<class Iter1, class Iter2>
    void test_iter()
    {
        int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
        const unsigned sa = sizeof(ia) / sizeof(ia[0]);
        int ib[] = {2, 4};
        const unsigned sb = sizeof(ib) / sizeof(ib[0]);
        int ic[] = {1, 2};
        int id[] = {3, 3, 3, 3};

        auto includes = make_testable_2<true, true>(ranges::includes);

        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib)).check(true_);
        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib + 1)).check(false_);
        includes(Iter1(ia), Iter1(ia + 1), Iter2(ib), Iter2(ib)).check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(ia), Iter2(ia + sa)).check(true_);

        includes(Iter1(ia), Iter1(ia + sa), Iter2(ib), Iter2(ib + sb)).check(true_);
        includes(Iter1(ib), Iter1(ib + sb), Iter2(ia), Iter2(ia + sa)).check(false_);

        includes(Iter1(ia), Iter1(ia + 2), Iter2(ic), Iter2(ic + 2)).check(true_);
        includes(Iter1(ia), Iter1(ia + 2), Iter2(ib), Iter2(ib + 2)).check(false_);

        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 1)).check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 2)).check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 3)).check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 4)).check(false_);
    }

    template<class Iter1, class Iter2>
    void test_comp()
    {
        int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
        const unsigned sa = sizeof(ia) / sizeof(ia[0]);
        int ib[] = {2, 4};
        const unsigned sb = sizeof(ib) / sizeof(ib[0]);
        int ic[] = {1, 2};
        int id[] = {3, 3, 3, 3};

        auto includes = make_testable_2<true, true>(ranges::includes);

        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib + 1), eastl::less<int>())
            .check(false_);
        includes(Iter1(ia), Iter1(ia + 1), Iter2(ib), Iter2(ib), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(ia), Iter2(ia + sa), eastl::less<int>())
            .check(true_);

        includes(Iter1(ia), Iter1(ia + sa), Iter2(ib), Iter2(ib + sb), eastl::less<int>())
            .check(true_);
        includes(Iter1(ib), Iter1(ib + sb), Iter2(ia), Iter2(ia + sa), eastl::less<int>())
            .check(false_);

        includes(Iter1(ia), Iter1(ia + 2), Iter2(ic), Iter2(ic + 2), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia + 2), Iter2(ib), Iter2(ib + 2), eastl::less<int>())
            .check(false_);

        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 1), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 2), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 3), eastl::less<int>())
            .check(true_);
        includes(Iter1(ia), Iter1(ia + sa), Iter2(id), Iter2(id + 4), eastl::less<int>())
            .check(false_);
    }

    template<class Iter1, class Iter2>
    void test()
    {
        test_iter<Iter1, Iter2>();
        test_comp<Iter1, Iter2>();
    }

    struct S
    {
        int i;
    };

    struct T
    {
        int j;
    };
} // namespace

int main()
{
    test<InputIterator<const int *>, InputIterator<const int *>>();
    test<InputIterator<const int *>, ForwardIterator<const int *>>();
    test<InputIterator<const int *>, BidirectionalIterator<const int *>>();
    test<InputIterator<const int *>, RandomAccessIterator<const int *>>();
    test<InputIterator<const int *>, const int *>();

    test<ForwardIterator<const int *>, InputIterator<const int *>>();
    test<ForwardIterator<const int *>, ForwardIterator<const int *>>();
    test<ForwardIterator<const int *>, BidirectionalIterator<const int *>>();
    test<ForwardIterator<const int *>, RandomAccessIterator<const int *>>();
    test<ForwardIterator<const int *>, const int *>();

    test<BidirectionalIterator<const int *>, InputIterator<const int *>>();
    test<BidirectionalIterator<const int *>, ForwardIterator<const int *>>();
    test<BidirectionalIterator<const int *>, BidirectionalIterator<const int *>>();
    test<BidirectionalIterator<const int *>, RandomAccessIterator<const int *>>();
    test<BidirectionalIterator<const int *>, const int *>();

    test<RandomAccessIterator<const int *>, InputIterator<const int *>>();
    test<RandomAccessIterator<const int *>, ForwardIterator<const int *>>();
    test<RandomAccessIterator<const int *>, BidirectionalIterator<const int *>>();
    test<RandomAccessIterator<const int *>, RandomAccessIterator<const int *>>();
    test<RandomAccessIterator<const int *>, const int *>();

    test<const int *, InputIterator<const int *>>();
    test<const int *, ForwardIterator<const int *>>();
    test<const int *, BidirectionalIterator<const int *>>();
    test<const int *, RandomAccessIterator<const int *>>();
    test<const int *, const int *>();

    // Test projections
    {
        S ia[] = {{1}, {2}, {2}, {3}, {3}, {3}, {4}, {4}, {4}, {4}};
        T id[] = {{3}, {3}, {3}};
        CHECK(ranges::includes(ia, id, eastl::less<int>(), &S::i, &T::j));
    }

    {
        using IL = std::initializer_list<int>;
        STATIC_CHECK(ranges::includes(IL{1, 2, 2, 3, 3, 3, 4, 4, 4, 4}, IL{3, 3, 3}, eastl::less<int>()));
    }

    return ::test_result();
}
