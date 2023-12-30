// Range v3 library
//
//  Copyright Johel Guerrero 2019
//  Copyright Google LLC 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <EASTL/array.h>
#include <EASTL/initializer_list.h>

#include <EARanges/algorithm/contains_subrange.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/view/subrange.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"

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

int comparison_count = 0;

template<typename T>
bool counting_equals(const T & a, const T & b)
{
    ++comparison_count;
    return a == b;
}

namespace {

template <class T, std::size_t N>
struct array
{
    T elems[N];
    T* begin() { return elems; }
    T* end() { return elems + N; }
};

}

int main()
{
    using namespace ranges;
    auto full_range = array<int, 10>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    auto valid_subrange = array<int, 5>{{2, 3, 4, 5, 6}};
    auto invalid_subrange = array<int, 5>{{3, 4, 5, 6, 2}};
    CHECK(contains_subrange(ForwardIterator<const int *>(full_range.begin()),
                            Sentinel<const int *>(full_range.end()),
                            ForwardIterator<const int *>(valid_subrange.begin()),
                            Sentinel<const int *>(valid_subrange.end())));
    CHECK(!contains_subrange(ForwardIterator<const int *>(full_range.begin()),
                             Sentinel<const int *>(full_range.end()),
                             ForwardIterator<const int *>(invalid_subrange.begin()),
                             Sentinel<const int *>(invalid_subrange.end())));
    CHECK(!contains_subrange(full_range, invalid_subrange));
    CHECK(contains_subrange(
        make_subrange(ForwardIterator<const int *>(full_range.begin()),
                      Sentinel<const int *>(full_range.end())),
        make_subrange(ForwardIterator<const int *>(valid_subrange.begin()),
                      Sentinel<const int *>(valid_subrange.end()))));

    comparison_count = 0;
    CHECK(!contains_subrange(RandomAccessIterator<const int*>(valid_subrange.begin()),
                             RandomAccessIterator<const int*>(valid_subrange.end()),
                             RandomAccessIterator<const int*>(full_range.begin()),
                             RandomAccessIterator<const int*>(full_range.end()),
                             counting_equals<int>));
    CHECK(comparison_count == 0);

    comparison_count = 0;
    CHECK(contains_subrange(ForwardIterator<const int *>(full_range.begin()),
                            Sentinel<const int *>(full_range.end()),
                            ForwardIterator<const int *>(valid_subrange.begin()),
                            Sentinel<const int *>(valid_subrange.end()),
                            counting_equals<int>));
    CHECK(comparison_count > 0);

    comparison_count = 0;
    CHECK(contains_subrange(make_subrange(ForwardIterator<const int *>(full_range.begin()),
                                          Sentinel<const int *>(full_range.end())),
                            make_subrange(ForwardIterator<const int *>(valid_subrange.begin()),
                                          Sentinel<const int *>(valid_subrange.end())),
                            counting_equals<int>));
    CHECK(comparison_count > 0);

    comparison_count = 0;
    CHECK(!contains_subrange(
        make_subrange(ForwardIterator<const int *>(full_range.begin()),
                      Sentinel<const int *>(full_range.begin())),
        make_subrange(ForwardIterator<const int *>(valid_subrange.begin()),
                      Sentinel<const int *>(valid_subrange.end())),
        counting_equals<int>));
    CHECK(comparison_count == 0);
    //TODO:7) Won't compile because of tuple shenaningans
#if EARANGES_CXX_CONSTEXPR >= EARANGES_CXX_CONSTEXPR_14 && EARANGES_CONSTEXPR_INVOKE
    using IL = std::initializer_list<int>;
    //static_assert(contains_subrange(IL{0, 1, 2, 3, 4}, IL{3, 4}), "");
    //static_assert(!contains_subrange(IL{0, 1, 2, 3, 4}, IL{2, 8}), "");
    static_assert(contains_subrange(IL{}, IL{}), "");
#endif

    return ::test_result();
}
