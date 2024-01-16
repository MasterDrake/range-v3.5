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

#include <EASTL/iterator.h>
#include <EASTL/slist.h>
#include <EASTL/string.h>
#include <EASTL/functional.h>
#include <EASTL/vector.h>

#include <EARanges/core.hpp>
#include <EARanges/view/join.hpp>
#include <EARanges/view/split.hpp>
#include <EARanges/view/generate_n.hpp>
#include <EARanges/view/repeat_n.hpp>
#include <EARanges/view/chunk.hpp>
#include <EARanges/view/concat.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/single.hpp>
#include <EARanges/view/transform.hpp>
#include <EARanges/view/filter.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

EARANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

namespace
{
    template<typename T, std::size_t N>
    struct input_array
    {
        T elements_[N];

        InputIterator<T*> begin() { return InputIterator<T*>{elements_ + 0}; }
        InputIterator<T*> end() { return InputIterator<T*>{elements_ + N}; }
        constexpr std::size_t size() const { return N; }
    };

    static int N = 0;
    auto make_input_rng()
    {
        using ranges::views::generate_n;
        return generate_n([](){
            return generate_n([](){
                return N++;
            },3);
        },3);
    }

    template<typename T>
    constexpr auto twice(T t)
    {
        return ranges::views::concat(
            ranges::views::single(t),
            ranges::views::single(t));
    }

#ifdef __clang__
EARANGES_DIAGNOSTIC_IGNORE_PRAGMAS
EARANGES_DIAGNOSTIC_IGNORE("-Wunneeded-member-function")
EARANGES_DIAGNOSTIC_IGNORE("-Wunused-member-function")
#endif

    // https://github.com/ericniebler/range-v3/issues/283
    void test_issue_283()
    {
        const eastl::vector<eastl::vector<int>> nums =
        {
            { 1, 2, 3 },
            { 4, 5, 6 }
        };
        const auto flat_nums = ranges::views::join( nums ) | ranges::to<eastl::vector>();
        ::check_equal(flat_nums, {1,2,3,4,5,6});
    }

    // https://github.com/ericniebler/range-v3/issues/1414
    void test_issue_1414()
    {
        eastl::slist<char> u2; // this can also be a vector
        eastl::vector<char> i2;
        auto v2 = u2 | ranges::views::chunk(3) | ranges::views::join(i2);
        CPP_assert(ranges::input_range<decltype(v2)>);
    }
}

int main()
{
    using namespace ranges;

    // Test that we can join an input range of input ranges:
    {
        auto rng0 = make_input_rng() | views::join;
        static_assert(range_cardinality<decltype(rng0)>::value == ranges::finite, "");
        CPP_assert(input_range<decltype(rng0)>);
        CPP_assert(!forward_range<decltype(rng0)>);
        CPP_assert(!common_range<decltype(rng0)>);
        CPP_assert(!sized_range<decltype(rng0)>);
        check_equal(rng0, {0,1,2,3,4,5,6,7,8});
    }

    // Joining with a value
    {
        N = 0;
        auto rng1 = make_input_rng() | views::join(42);
        static_assert(range_cardinality<decltype(rng1)>::value == ranges::finite, "");
        CPP_assert(input_range<decltype(rng1)>);
        CPP_assert(!forward_range<decltype(rng1)>);
        CPP_assert(!common_range<decltype(rng1)>);
        CPP_assert(!sized_range<decltype(rng1)>);
        check_equal(rng1, {0,1,2,42,3,4,5,42,6,7,8});
    }

    // Joining with a range
    {
        N = 0;
        int rgi[] = {42,43};
        auto rng2 = make_input_rng() | views::join(rgi);
        static_assert(range_cardinality<decltype(rng2)>::value == ranges::finite, "");
        CPP_assert(input_range<decltype(rng2)>);
        CPP_assert(!forward_range<decltype(rng2)>);
        CPP_assert(!common_range<decltype(rng2)>);
        CPP_assert(!sized_range<decltype(rng2)>);
        check_equal(rng2, {0,1,2,42,43,3,4,5,42,43,6,7,8});
    }

    // Just for fun:
    //TODO:35) yeah, fixing this eastl::string conversion ain't fun at all, Mr. Niebler
    /*{
        eastl::string str = "Now,is,the,time,for,all,good,men,to,come,to,the,aid,of,their,country";
        auto res = str | views::split(',') | views::join(' ') | to<eastl::string>();
        CHECK(res == "Now is the time for all good men to come to the aid of their country");
        static_assert(range_cardinality<decltype(res)>::value == ranges::finite, "");
    }*/

    /* {
        eastl::vector<eastl::string> vs{"This","is","his","face"};
        auto rng3 = views::join(vs);
        static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
        CPP_assert(!sized_range<decltype(rng3)>);
        CPP_assert(!sized_sentinel_for<decltype(end(rng3)), decltype(begin(rng3))>);
        CHECK(to<eastl::string>(rng3) == "Thisishisface");

        auto rng4 = views::join(vs, ' ');
        static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
        CPP_assert(!sized_range<decltype(rng4)>);
        CPP_assert(!sized_sentinel_for<decltype(end(rng4)), decltype(begin(rng4))>);
        CHECK(to<eastl::string>(rng4) == "This is his face");
    }*/

    {
        auto rng5 = views::join(twice(twice(42)));
        static_assert(range_cardinality<decltype(rng5)>::value == 4, "");
        CPP_assert(sized_range<decltype(rng5)>);
        CHECK(rng5.size() == 4u);
        check_equal(rng5, {42,42,42,42});
    }

    {
        auto rng6 = views::join(twice(views::repeat_n(42, 2)));
        static_assert(range_cardinality<decltype(rng6)>::value == ranges::finite, "");
        CPP_assert(sized_range<decltype(rng6)>);
        CHECK(rng6.size() == 4u);
        check_equal(rng6, {42,42,42,42});
    }

    {
        input_array<eastl::string, 4> some_strings = {{"This","is","his","face"}};
        CPP_assert(input_range<decltype(some_strings)>);
        CPP_assert(sized_range<decltype(some_strings)>);
        CPP_assert(!sized_range<decltype(some_strings | views::join)>);
    }

    {
        int const some_int_pairs[3][2] = {{0,1},{2,3},{4,5}};
        auto rng = debug_input_view<int const[2]>{some_int_pairs} | views::join;
        check_equal(rng, {0,1,2,3,4,5});
    }

    {
        eastl::vector<eastl::string> vs{"this","is","his","face"};
        join_view<ref_view<eastl::vector<eastl::string>>> jv{vs};
        check_equal(jv, {'t','h','i','s','i','s','h','i','s','f','a','c','e'});
        CPP_assert(bidirectional_range<decltype(jv)>);
        CPP_assert(bidirectional_range<const decltype(jv)>);
        CPP_assert(common_range<decltype(jv)>);
        CPP_assert(common_range<const decltype(jv)>);
    }

    {
        auto rng = views::iota(0,4)
            | views::transform([](int i) {return views::iota(0,i);})
            | views::join;
        check_equal(rng, {0,0,1,0,1,2});
        CPP_assert(input_range<decltype(rng)>);
        CPP_assert(!range<const decltype(rng)>);
        CPP_assert(!forward_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
    }

    {
        auto rng = views::iota(0,4)
            | views::transform([](int i) {return views::iota(0,i);})
            | views::filter([](auto){ return true; })
            | views::join;
        check_equal(rng, {0,0,1,0,1,2});
        CPP_assert(input_range<decltype(rng)>);
        CPP_assert(!range<const decltype(rng)>);
        CPP_assert(!forward_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
    }

    {
        auto rng = views::iota(0,4)
            | views::transform([](int i) {return eastl::string((std::size_t) i, char('a'+i));})
            | views::join;
        check_equal(rng, {'b','c','c','d','d','d'});
        CPP_assert(input_range<decltype(rng)>);
        CPP_assert(!range<const decltype(rng)>);
        CPP_assert(!forward_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
    }

    {
        auto rng = views::iota(0,4)
            | views::transform([](int i) {return eastl::string((std::size_t) i, char('a'+i));})
            | views::join('-');
        check_equal(rng, {'-','b','-','c','c','-','d','d','d'});
        CPP_assert(input_range<decltype(rng)>);
        CPP_assert(!range<const decltype(rng)>);
        CPP_assert(!forward_range<decltype(rng)>);
        CPP_assert(!common_range<decltype(rng)>);
    }

    // https://github.com/ericniebler/range-v3/issues/1320
    {
        auto op = [](auto & input, int i, auto & ins)
        {
            return input | ranges::views::chunk(i)
                         | ranges::views::join(ins);
        };
        eastl::string input{"foobarbaxbat"};
        eastl::string insert{"X"};
        auto rng = op(input, 2, insert);
        std::cout << rng << '\n';
        ::check_equal(rng, {'f','o','X','o','b','X','a','r','X','b','a','X','x','b','X','a','t'});
    }

    {
        auto op = [](auto & input, int i, auto & ins)
        {
            return input | ranges::views::chunk(i) | ranges::views::join(ins);
        };
        eastl::vector<eastl::string> input{"foo","bar","bax","bat"};
        eastl::string insert{"XX"};
        auto rng = op(input, 2, insert);
        std::cout << rng << '\n';
        ::check_equal(rng, {"foo","bar","XX","bax","bat"});
    }

    {
        eastl::vector<int> v = {1, 2, 3};
        auto throws = [](auto &&) -> eastl::vector<eastl::vector<int>> & { throw 42; };

        auto rng = v | ranges::views::transform(throws) | ranges::views::join;
        try
        {
            auto d = ranges::distance(rng);
            CHECK(false);
            CHECK(d == 3);
        }
        catch(int)
        {}
        catch(...)
        {
            CHECK(false);
        }
    }

    test_issue_283();
    test_issue_1414();

    return ::test_result();
}
