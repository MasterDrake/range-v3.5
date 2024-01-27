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

#include <EASTL/algorithm.h>
#include <EASTL/memory.h>
#include <sstream>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/for_each.hpp>
#include <EARanges/algorithm/find_if.hpp>
#include <EARanges/utility/copy.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/insert_iterators.hpp>
#include <EARanges/view/common.hpp>
#include <EARanges/view/filter.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/map.hpp>
#include <EARanges/view/move.hpp>
#include <EARanges/view/stride.hpp>
#include <EARanges/view/take_while.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/zip.hpp>
#include <EARanges/view/zip_with.hpp>
#include <EARanges/view/all.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

//TODO: Some test fails, I guess it has to do with eastl::string...

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201911
// See https://github.com/ericniebler/range-v3/issues/1480
void test_bug1480()
{
    eastl::vector<char> const first{};
    eastl::vector<char> const second{};

    auto zip_view = ::ranges::views::zip(first, second);
    auto fn = [&] ([[maybe_unused]] auto && ch)
    {
    };
    ranges::for_each(zip_view, fn);
}
#endif

int main()
{
    using namespace ranges;

    eastl::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    eastl::vector<eastl::string> const vs{"hello", "goodbye", "hello", "goodbye"};

    // All common ranges, but one single-pass
    {
        std::stringstream str{"john paul george ringo"};
        using V = eastl::tuple<int, eastl::string, eastl::string>;
        auto rng = views::zip(vi, vs, istream<eastl::string>(str) | views::common);
        using Rng = decltype(rng);
        CPP_assert(view_<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
        CPP_assert(!sized_range<decltype(rng)>);
        CPP_assert(same_as<range_value_t<Rng>, eastl::tuple<int, eastl::string, eastl::string>>);
        CPP_assert(same_as<range_reference_t<Rng>, common_tuple<int &, eastl::string const &, eastl::string &>>);
        CPP_assert(same_as<range_rvalue_reference_t<Rng>, common_tuple<int &&, eastl::string const &&, eastl::string &&>>);
        CPP_assert(convertible_to<range_value_t<Rng> &&, range_rvalue_reference_t<Rng>>);
        CPP_assert(input_iterator<decltype(begin(rng))>);
        CPP_assert(!forward_iterator<decltype(begin(rng))>);
        has_cardinality<cardinality::finite>(rng);
        auto expected = to_vector(rng);
        ::check_equal(expected, {V{0, "hello", "john"},
                                 V{1, "goodbye", "paul"},
                                 V{2, "hello", "george"},
                                 V{3, "goodbye", "ringo"}});
    }

    // Mixed ranges and common ranges
    {
        std::stringstream str{"john paul george ringo"};
        using V = eastl::tuple<int, eastl::string, eastl::string>;
        auto rng = views::zip(vi, vs, istream<eastl::string>(str));
        CPP_assert(view_<decltype(rng)>);
        CPP_assert(!sized_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
        CPP_assert(input_iterator<decltype(begin(rng))>);
        CPP_assert(!forward_iterator<decltype(begin(rng))>);
        has_cardinality<cardinality::finite>(rng);
        eastl::vector<V> expected;
        ranges::copy(rng, ranges::back_inserter(expected));
        ::check_equal(expected, {V{0, "hello", "john"},
                                 V{1, "goodbye", "paul"},
                                 V{2, "hello", "george"},
                                 V{3, "goodbye", "ringo"}});
    }

    auto rnd_rng = views::zip(vi, vs);
    using Ref = range_reference_t<decltype(rnd_rng)>;
    static_assert(eastl::is_same<Ref, common_pair<int &,eastl::string const &>>::value, "");
    CPP_assert(view_<decltype(rnd_rng)>);
    CPP_assert(common_range<decltype(rnd_rng)>);
    CPP_assert(sized_range<decltype(rnd_rng)>);
    CPP_assert(random_access_iterator<decltype(begin(rnd_rng))>);
    has_cardinality<cardinality::finite>(rnd_rng);
    auto tmp = cbegin(rnd_rng) + 3;
    CHECK(eastl::get<0>(*tmp) == 3);
    CHECK(eastl::get<1>(*tmp) == "goodbye");

    CHECK((rnd_rng.end() - rnd_rng.begin()) == 4);
    CHECK((rnd_rng.begin() - rnd_rng.end()) == -4);
    CHECK(rnd_rng.size() == 4u);

    // zip_with
    {
        eastl::vector<eastl::string> v0{"a","b","c"};
        eastl::vector<eastl::string> v1{"x","y","z"};

        auto rng = views::zip_with(eastl::plus<eastl::string>{}, v0, v1);
        eastl::vector<eastl::string> expected;
        copy(rng, ranges::back_inserter(expected));
        ::check_equal(expected, {"ax","by","cz"});

        auto rng2 = views::zip_with([] { return 42; });
        static_assert(eastl::is_same<range_value_t<decltype(rng2)>, int>::value, "");
    }

    // Move from a zip view
    {
        auto v0 = to<eastl::vector<MoveOnlyString>>({"a","b","c"});
        auto v1 = to<eastl::vector<MoveOnlyString>>({"x","y","z"});

        auto rng = views::zip(v0, v1);
        CPP_assert(random_access_range<decltype(rng)>);
        eastl::vector<eastl::pair<MoveOnlyString, MoveOnlyString>> expected;
        move(rng, ranges::back_inserter(expected));
        ::check_equal(expected | views::keys, {"a","b","c"});
        ::check_equal(expected | views::values, {"x","y","z"});
        ::check_equal(v0, {"","",""});
        ::check_equal(v1, {"","",""});

        move(expected, rng.begin());
        ::check_equal(expected | views::keys, {"","",""});
        ::check_equal(expected | views::values, {"","",""});
        ::check_equal(v0, {"a","b","c"});
        ::check_equal(v1, {"x","y","z"});

        eastl::vector<MoveOnlyString> res;
        using R = decltype(rng);
        auto proj =
            [](range_reference_t<R> p) -> MoveOnlyString& {return p.first;};
        auto rng2 = rng | views::transform(proj);
        move(rng2, ranges::back_inserter(res));
        ::check_equal(res, {"a","b","c"});
        ::check_equal(v0, {"","",""});
        ::check_equal(v1, {"x","y","z"});
        using R2 = decltype(rng2);
        CPP_assert(same_as<range_value_t<R2>, MoveOnlyString>);
        CPP_assert(same_as<range_reference_t<R2>, MoveOnlyString &>);
        CPP_assert(same_as<range_rvalue_reference_t<R2>, MoveOnlyString &&>);
    }

    {
        auto const v = to<eastl::vector<MoveOnlyString>>({"a","b","c"});
        auto rng = views::zip(v, v);
        using Rng = decltype(rng);
        using I = iterator_t<Rng>;
        CPP_assert(indirectly_readable<I>);
        CPP_assert(same_as<
            range_value_t<Rng>,
            eastl::pair<MoveOnlyString, MoveOnlyString>>);
        CPP_assert(same_as<
            range_reference_t<Rng>,
            common_pair<MoveOnlyString const &, MoveOnlyString const &>>);
        CPP_assert(same_as<
            range_rvalue_reference_t<Rng>,
            common_pair<MoveOnlyString const &&, MoveOnlyString const &&>>);
        CPP_assert(same_as<
            range_common_reference_t<Rng>,
            common_pair<MoveOnlyString const &, MoveOnlyString const &>>);
    }

    {
        eastl::vector<int> v{1,2,3,4};
        auto moved = v | views::move;
        using Moved = decltype(moved);
        CPP_assert(same_as<range_reference_t<Moved>, int &&>);
        auto zipped = views::zip(moved);
        using Zipped = decltype(zipped);
        CPP_assert(same_as<range_reference_t<Zipped>, common_tuple<int &&> >);
    }

    // This is actually a test of the logic of view_adaptor. Since the stride view
    // does not redefine the current member function, the base range's iter_move
    // function gets picked up automatically.
    {
        auto rng0 = views::zip(vi, vs);
        auto rng1 = views::stride(rng0, 2);
        CPP_assert(same_as<range_rvalue_reference_t<decltype(rng1)>, range_rvalue_reference_t<decltype(rng0)>>);
        CPP_assert(same_as<range_value_t<decltype(rng1)>, range_value_t<decltype(rng0)>>);
    }

    // Test for noexcept iter_move
    {
        static_assert(noexcept(eastl::declval<eastl::unique_ptr<int>&>() = eastl::declval<eastl::unique_ptr<int>&&>()), "");
        eastl::unique_ptr<int> rg1[10], rg2[10];
        auto x = views::zip(rg1, rg2);
        eastl::pair<eastl::unique_ptr<int>, eastl::unique_ptr<int>> p = iter_move(x.begin());
        auto it = x.begin();
        static_assert(noexcept(ranges::iter_move(it)), "");
    }

    // Really a test for common_iterator's iter_move, but this is a good place for it.
    {
        eastl::unique_ptr<int> rg1[10], rg2[10];
        auto rg3 = rg2 | views::take_while([](eastl::unique_ptr<int> &){return true;});
        auto x = views::zip(rg1, rg3);
        CPP_assert(!common_range<decltype(x)>);
        auto y = x | views::common;
        eastl::pair<eastl::unique_ptr<int>, eastl::unique_ptr<int>> p = iter_move(y.begin());
        auto it = x.begin();
        static_assert(noexcept(iter_move(it)), "");
    }

    // Regression test for #439.
    {
        eastl::vector<int> vec{0,1,2};
        auto rng = vec | views::for_each([](int i) { return ranges::yield(i); });
        ranges::distance(views::zip(views::iota(0), rng) | views::common);
    }

    {
        int const i1[] = {0,1,2,3};
        int const i2[] = {4,5,6,7};
        auto rng = views::zip(debug_input_view<int const>{i1}, debug_input_view<int const>{i2});
        using P = eastl::pair<int, int>;
        has_cardinality<cardinality::finite>(rng);
        ::check_equal(rng, {P{0,4},P{1,5}, P{2,6}, P{3,7}});
    }

    {
        // Test with no ranges
        auto rng = views::zip();
        using R = decltype(rng);
        CPP_assert(same_as<range_value_t<R>, eastl::tuple<>>);
        CPP_assert(contiguous_range<R>);
        has_cardinality<cardinality(0)>(rng);
        CHECK(ranges::begin(rng) == ranges::end(rng));
        CHECK(ranges::size(rng) == 0u);
    }

    {
        // test dangling
        auto true_ = [](auto&&){ return true; };

        CHECK(!::is_dangling(ranges::find_if(views::zip(vi, vs), true_)));
        CHECK(!::is_dangling(ranges::find_if(views::zip(
            vi | views::move,
            vs | views::common
            ), true_)));
        CHECK(::is_dangling(ranges::find_if(views::zip(
            vi | views::filter(true_)), true_)));
    }

    {
        // test zip with infinite range
        int i1[] = {0,1,2,3};
        auto rng = views::zip(i1, views::iota(4));

        has_cardinality<cardinality(4)>(rng);
        using P = eastl::pair<int, int>;
        ::check_equal(rng, {P{0,4},P{1,5}, P{2,6}, P{3,7}});
    }

    {
        // test zip with infinite ranges only
        auto rng = views::zip(views::iota(0), views::iota(4));

        has_cardinality<cardinality::infinite>(rng);
        using P = eastl::pair<int, int>;
        ::check_equal(rng | views::take(4), {P{0,4},P{1,5}, P{2,6}, P{3,7}});
    }

    {
        // test unknown cardinality
        std::stringstream str{};
        auto rng = views::zip(istream<eastl::string>(str));

        has_cardinality<cardinality::unknown>(rng);
    }
    
    {
        eastl::vector<int> v0{1, 2, 3};
        eastl::vector<int> v1{};

        auto rng0 = views::zip(v0, v1);
        auto rng1 = views::zip(v1, v0);

        CHECK(ranges::distance(ranges::begin(rng0), ranges::end(rng0)) == 0);
        CHECK(ranges::distance(ranges::begin(rng1), ranges::end(rng1)) == 0);
        CHECK(ranges::distance(ranges::end(rng0), ranges::begin(rng0)) == 0);
        CHECK(ranges::distance(ranges::end(rng1), ranges::begin(rng1)) == 0);
    }

    {
        eastl::vector<int> v0{1, 2, 3};
        eastl::vector<int> v1{1, 2, 3, 4, 5};

        auto rng = views::zip(v0, v1);

        CHECK(ranges::distance(ranges::begin(rng), ranges::end(rng)) == 3);
        CHECK(ranges::distance(ranges::end(rng), ranges::begin(rng)) == -3);
    }

    return test_result();
}
