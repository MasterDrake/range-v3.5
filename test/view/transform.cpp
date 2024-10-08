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
#include <EASTL/vector.h>
#include <EASTL/iterator.h>
#include <EASTL/functional.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/functional/overload.hpp>
#include <EARanges/iterator/insert_iterators.hpp>
#include <EARanges/utility/copy.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/view/counted.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/span.hpp>
#include <EARanges/view/zip.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct is_odd
{
    bool operator()(int i) const
    {
        return (i % 2) == 1;
    }
};

// https://github.com/ericniebler/range-v3/issues/996
void bug_996()
{
    eastl::vector<int> buff(12, -1);
    ::ranges::span<int> sp(buff.data(), 12);

    auto x = ::ranges::views::transform(sp, [](int a) { return a > 3 ? a : 42; });
    auto y = ::ranges::views::transform(x, sp, [](int a, int b) { return a + b; });
    auto rng = ::ranges::views::transform(y, [](int a) { return a + 1; });
    (void)rng;
}

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    eastl::pair<int, int> rgp[] = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {7,7}, {8,8}, {9,9}, {10,10}};

    {
        auto rng = rgi | views::transform(is_odd());
        has_type<int &>(*begin(rgi));
        has_type<bool>(*begin(rng));
        CPP_assert(view_<decltype(rng)>);
        CPP_assert(sized_range<decltype(rng)>);
        CPP_assert(random_access_range<decltype(rng)>);
        ::check_equal(rng, {true, false, true, false, true, false, true, false, true, false});
    }

    {
        auto rng2 = rgp | views::transform(&eastl::pair<int,int>::first);
        has_type<int &>(*begin(rng2));
        CPP_assert(same_as<range_value_t<decltype(rng2)>, int>);
        CPP_assert(same_as<decltype(iter_move(begin(rng2))), int &&>);
        CPP_assert(view_<decltype(rng2)>);
        CPP_assert(common_range<decltype(rng2)>);
        CPP_assert(sized_range<decltype(rng2)>);
        CPP_assert(random_access_range<decltype(rng2)>);
        ::check_equal(rng2, {1,2,3,4,5,6,7,8,9,10});
        ::check_equal(rng2 | views::reverse, {10,9,8,7,6,5,4,3,2,1});
        CHECK(&*begin(rng2) == &rgp[0].first);
        CHECK(rng2.size() == 10u);
    }

    {

        auto rng3 = views::counted(rgp, 10) | views::transform(&eastl::pair<int,int>::first);
        has_type<int &>(*begin(rng3));
        CPP_assert(view_<decltype(rng3)>);
        CPP_assert(common_range<decltype(rng3)>);
        CPP_assert(sized_range<decltype(rng3)>);
        CPP_assert(random_access_range<decltype(rng3)>);
        ::check_equal(rng3, {1,2,3,4,5,6,7,8,9,10});
        CHECK(&*begin(rng3) == &rgp[0].first);
        CHECK(rng3.size() == 10u);
    }

    {
        auto rng4 = views::counted(ForwardIterator<eastl::pair<int, int>*>{rgp}, 10)
                        | views::transform(&eastl::pair<int,int>::first);
        has_type<int &>(*begin(rng4));
        CPP_assert(view_<decltype(rng4)>);
        CPP_assert(!common_range<decltype(rng4)>);
        CPP_assert(sized_range<decltype(rng4)>);
        CPP_assert(forward_range<decltype(rng4)>);
        CPP_assert(!bidirectional_range<decltype(rng4)>);
        ::check_equal(rng4, {1,2,3,4,5,6,7,8,9,10});
        CHECK(&*begin(rng4) == &rgp[0].first);
        CHECK(rng4.size() == 10u);

        counted_iterator<ForwardIterator<eastl::pair<int, int>*>> i = begin(rng4).base();
        (void)i;
    }

    // Test transform with a mutable lambda
    {
        int cnt = 100;
        auto mutable_rng = views::transform(rgi, [cnt](int) mutable { return cnt++;});
        ::check_equal(mutable_rng, {100,101,102,103,104,105,106,107,108,109});
        CHECK(cnt == 100);
        CPP_assert(view_<decltype(mutable_rng)>);
        CPP_assert(!view_<decltype(mutable_rng) const>);
    }

    // Test iter_transform by transforming a zip view to select one element.
    {
        auto v0 = to<eastl::vector<MoveOnlyString>>({"a","b","c"});
        auto v1 = to<eastl::vector<MoveOnlyString>>({"x","y","z"});

        auto rng1 = views::zip(v0, v1);
        CPP_assert(random_access_range<decltype(rng1)>);

        eastl::vector<MoveOnlyString> res;
        using R1 = decltype(rng1);
        using I1 = iterator_t<R1>;
        // Needlessly verbose -- a simple transform would do the same, but this is an interesting test.
        auto proj = overload(
            [](I1 i1) -> MoveOnlyString& {return (*i1).first;},
            [](copy_tag, I1) -> MoveOnlyString {return {};},
            [](move_tag, I1 i1) -> MoveOnlyString&& {return eastl::move((*i1).first);}
        );
        auto rng2 = rng1 | views::iter_transform(proj);
        move(rng2, ranges::back_inserter(res));
        ::check_equal(res, {"a","b","c"});
        ::check_equal(v0, {"","",""});
        ::check_equal(v1, {"x","y","z"});
        using R2 = decltype(rng2);
        CPP_assert(same_as<range_value_t<R2>, MoveOnlyString>);
        CPP_assert(same_as<range_reference_t<R2>, MoveOnlyString &>);
        CPP_assert(same_as<range_rvalue_reference_t<R2>, MoveOnlyString &&>);
    }

    // two range transform
    {
        auto v0 = to<eastl::vector<eastl::string>>({"a","b","c"});
        auto v1 = to<eastl::vector<eastl::string>>({"x","y","z"});

        auto rng = views::transform(v0, v1, [](eastl::string& s0, eastl::string& s1){return eastl::tie(s0, s1);});
        using R = decltype(rng);
        CPP_assert(same_as<range_value_t<R>, eastl::tuple<eastl::string&, eastl::string&>>);
        CPP_assert(same_as<range_reference_t<R>, eastl::tuple<eastl::string&, eastl::string&>>);
        CPP_assert(same_as<range_rvalue_reference_t<R>, eastl::tuple<eastl::string&, eastl::string&>>);

        using T = eastl::tuple<eastl::string, eastl::string>;
        ::check_equal(rng, {T{"a","x"}, T{"b","y"}, T{"c","z"}});
    }

    // two range indirect transform
    {
        auto v0 = to<eastl::vector<eastl::string>>({"a","b","c"});
        auto v1 = to<eastl::vector<eastl::string>>({"x","y","z"});
        using I = eastl::vector<eastl::string>::iterator;

        auto fun = overload(
            [](I i, I j)           { return eastl::tie(*i, *j); },
            [](copy_tag, I, I)     { return eastl::tuple<eastl::string, eastl::string>{}; },
            [](move_tag, I i, I j) { return common_tuple<eastl::string&&, eastl::string&&>{
                eastl::move(*i), eastl::move(*j)}; } );

        auto rng = views::iter_transform(v0, v1, fun);
        using R = decltype(rng);
        CPP_assert(same_as<range_value_t<R>, eastl::tuple<eastl::string, eastl::string>>);
        CPP_assert(same_as<range_reference_t<R>, eastl::tuple<eastl::string&, eastl::string&>>);
        CPP_assert(same_as<range_rvalue_reference_t<R>, common_tuple<eastl::string&&, eastl::string&&>>);

        using T = eastl::tuple<eastl::string, eastl::string>;
        ::check_equal(rng, {T{"a","x"}, T{"b","y"}, T{"c","z"}});
    }

    {
        auto rng = debug_input_view<int const>{rgi} | views::transform(is_odd{});
        ::check_equal(rng, {true, false, true, false, true, false, true, false, true, false});
    }

    {
        auto v0 = to<eastl::vector<eastl::string>>({"a","b","c"});
        auto v1 = to<eastl::vector<eastl::string>>({"x","y","z"});

        auto r0 = debug_input_view<eastl::string>{v0.data(), distance(v0)};
        auto r1 = debug_input_view<eastl::string>{v1.data(), distance(v1)};
        auto rng = views::transform(eastl::move(r0), eastl::move(r1),
            [](eastl::string &s0, eastl::string &s1){ return eastl::tie(s0, s1); });
        using R = decltype(rng);
        CPP_assert(same_as<range_value_t<R>, eastl::tuple<eastl::string &, eastl::string &>>);
        CPP_assert(same_as<range_reference_t<R>, eastl::tuple<eastl::string &, eastl::string &>>);
        CPP_assert(same_as<range_rvalue_reference_t<R>, eastl::tuple<eastl::string &, eastl::string &>>);

        using T = eastl::tuple<eastl::string, eastl::string>;
        ::check_equal(rng, {T{"a","x"}, T{"b","y"}, T{"c","z"}});
    }

    {
#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
#if defined(__clang__) && __clang_major__ < 6
        // Workaround https://bugs.llvm.org/show_bug.cgi?id=33314
        EARANGES_DIAGNOSTIC_PUSH
        EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#endif
        eastl::vector<int> vi = {1, 2, 3};
        ranges::transform_view times_ten{vi, [](int i) { return i * 10; }};
        ::check_equal(times_ten, {10, 20, 30});
#if defined(__clang__) && __clang_major__ < 6
        EARANGES_DIAGNOSTIC_POP
#endif // clang bug workaround
#endif // use deduction guides
    }

    return test_result();
}
