// Range v3 library
//
//  Copyright Eric Niebler 2014, 2016
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <ostream>
#include <sstream>
#include <EASTL/list.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EARanges/meta/meta.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/move_iterators.hpp>
#include <EARanges/iterator/insert_iterators.hpp>
#include <EARanges/iterator/stream_iterators.hpp>
#include <EARanges/algorithm/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

struct MoveOnlyReadable
{
    using value_type = eastl::unique_ptr<int>;
    value_type operator*() const;
};

CPP_assert(indirectly_readable<MoveOnlyReadable>);

void test_insert_iterator()
{
    CPP_assert(output_iterator<insert_iterator<eastl::vector<int>>, int&&>);
    CPP_assert(!equality_comparable<insert_iterator<eastl::vector<int>>>);
    eastl::vector<int> vi{5,6,7,8};
    copy(std::initializer_list<int>{1,2,3,4}, inserter(vi, vi.begin()+2));
    ::check_equal(vi, {5,6,1,2,3,4,7,8});
}
//TODO:How it's possible that MSVC doesn't complain about eastl<->std string comparisons???
void test_ostream_joiner()
{
    std::ostringstream oss;
    eastl::vector<int> vi{};
    copy(vi, make_ostream_joiner(oss, ","));
    ::check_equal(eastl::string(oss.str().c_str(), oss.str().length()), eastl::string{""});
    vi = {1,2,3,4};
    copy(vi, make_ostream_joiner(oss, ","));
    ::check_equal(eastl::string(oss.str().c_str(), oss.str().length()), eastl::string{"1,2,3,4"});
}

void test_move_iterator()
{
    eastl::vector<MoveOnlyString> in;
    in.emplace_back("this");
    in.emplace_back("is");
    in.emplace_back("his");
    in.emplace_back("face");
    eastl::vector<MoveOnlyString> out;
    auto first = ranges::make_move_iterator(in.begin());
    using I = decltype(first);
    CPP_assert(input_iterator<I>);
    CPP_assert(!forward_iterator<I>);
    CPP_assert(same_as<I, ranges::move_iterator<eastl::vector<MoveOnlyString>::iterator>>);
    auto last = ranges::make_move_sentinel(in.end());
    using S = decltype(last);
    CPP_assert(sentinel_for<S, I>);
    CPP_assert(sized_sentinel_for<I, I>);
    CHECK((first - first) == 0);
    CPP_assert(sized_sentinel_for<S, I>);
    CHECK(static_cast<eastl::size_t>(last - first) == in.size());
    ranges::copy(first, last, ranges::back_inserter(out));
    ::check_equal(in, {"","","",""});
    ::check_equal(out, {"this","is","his","face"});
}

template<class I>
using RI = eastl::reverse_iterator<I>;

void issue_420_regression()
{
    // Verify that sized_sentinel_for<eastl::reverse_iterator<S>, eastl::reverse_iterator<I>> properly requires sized_sentinel_for<I, S>
    
    CPP_assert(sized_sentinel_for<RI<int*>, RI<int*>>);
    CPP_assert(!sized_sentinel_for<RI<int*>, RI<float*>>);
    using BI = BidirectionalIterator<int*>;
    CPP_assert(!sized_sentinel_for<RI<BI>, RI<BI>>);
}

struct value_type_tester_thingy {};

namespace ranges
{
    template<>
    struct indirectly_readable_traits<::value_type_tester_thingy>
    {
        using value_type = int;
    };
}

template<typename T>
struct with_value_type { using value_type = T; };
template<typename T>
struct with_element_type { using element_type = T; };

// arrays of known bound
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int[4]>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<const int[4]>::value_type>);
CPP_assert(same_as<int*, ranges::indirectly_readable_traits<int*[4]>::value_type>);
CPP_assert(same_as<with_value_type<int>, ranges::indirectly_readable_traits<with_value_type<int>[4]>::value_type>);

#if !defined(__GNUC__) || defined(__clang__)
// arrays of unknown bound
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int[]>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<const int[]>::value_type>);
#endif

template<typename T>
using readable_traits_value_type_t = typename ranges::indirectly_readable_traits<T>::value_type;
template<typename T>
using readable_traits_value_type = meta::defer<readable_traits_value_type_t, T>;

// object pointer types
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int*>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int*const>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int const*>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<int const*const>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<int(*)[4]>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<const int(*)[4]>::value_type>);
struct incomplete;
CPP_assert(same_as<incomplete, ranges::indirectly_readable_traits<incomplete*>::value_type>);
static_assert(!meta::is_trait<readable_traits_value_type<void*>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<void const*>>::value, "");

// class types with member value_type
CPP_assert(same_as<int, ranges::indirectly_readable_traits<with_value_type<int>>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<with_value_type<int> const>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<value_type_tester_thingy>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<value_type_tester_thingy const>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<with_value_type<int[4]>>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<with_value_type<int[4]> const>::value_type>);
static_assert(!meta::is_trait<readable_traits_value_type<with_value_type<void>>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_value_type<int(int)>>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_value_type<int&>>>::value, "");

// class types with member element_type
CPP_assert(same_as<int, ranges::indirectly_readable_traits<with_element_type<int>>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<with_element_type<int> const>::value_type>);
CPP_assert(same_as<int, ranges::indirectly_readable_traits<with_element_type<int const>>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<with_element_type<int[4]>>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<with_element_type<int[4]> const>::value_type>);
CPP_assert(same_as<int[4], ranges::indirectly_readable_traits<with_element_type<int const[4]>>::value_type>);
static_assert(!meta::is_trait<readable_traits_value_type<with_element_type<void>>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_element_type<void const>>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_element_type<void> const>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_element_type<int(int)>>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<with_element_type<int&>>>::value, "");

// cv-void
static_assert(!meta::is_trait<readable_traits_value_type<void>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<void const>>::value, "");
// reference types
static_assert(!meta::is_trait<readable_traits_value_type<int&>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<int&&>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<int*&>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<int*&&>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<int(&)(int)>>::value, "");
static_assert(!meta::is_trait<readable_traits_value_type<std::ostream&>>::value, "");

CPP_assert(indirectly_swappable<int *, int *>);
CPP_assert(indirectly_movable<int const *, int *>);
CPP_assert(!indirectly_swappable<int const *, int const *>);
CPP_assert(!indirectly_movable<int const *, int const *>);

namespace Boost
{
    struct S {}; // just to have a type from Boost namespace
    template<typename I, typename D>
    void advance(I&, D)
    {}
}

// Regression test for https://github.com/ericniebler/range-v3/issues/845
void test_845()
{
    eastl::list<eastl::pair<Boost::S, int>> v = { {Boost::S{}, 0} };
    auto itr = v.begin();
    ranges::advance(itr, 1); // Should not create ambiguity
}

// Test for https://github.com/ericniebler/range-v3/issues/1110
void test_1110()
{
    // this should not trigger assertation error
    eastl::vector<int> v = {1,2,3};
    auto e = ranges::end(v);
    ranges::advance(e, 0, ranges::begin(v));
}

int main()
{
    test_insert_iterator();
    test_move_iterator();
    test_ostream_joiner();
    issue_420_regression();
    test_1110();

    {
        struct S { using value_type = int; };
        CPP_assert(same_as<int, ranges::indirectly_readable_traits<S const>::value_type>);
    }

    return ::test_result();
}
