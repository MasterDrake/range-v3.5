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

#include <cctype>
#include <sstream>
#include <EARanges/core.hpp>
#include <EARanges/view/counted.hpp>
#include <EARanges/view/c_str.hpp>
#include <EARanges/view/empty.hpp>
#include <EARanges/view/remove_if.hpp>
#include <EARanges/view/split.hpp>
#include <EARanges/view/split_when.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

//TODO: BUGBUG views::split doesn't like istreamIterators and eastl::string because of char traits, but not sure if we should care or not...
EARANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

#if defined(__clang__) && __clang_major__ < 6
// Workaround https://bugs.llvm.org/show_bug.cgi?id=33314
EARANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE
#endif

namespace
{
    struct starts_with_g
    {
        template<typename I, typename S>
        eastl::pair<bool, I> operator()(I b, S) const
        {
            return {*b == 'g', b};
        }
    };

#ifdef EARANGES_WORKAROUND_MSVC_790554
    template<eastl::size_t N>
    auto c_str(char const (&sz)[N])
    {
        return ranges::subrange<char const*>{&sz[0], &sz[N-1]};
    }
#else // ^^^ workaround / no workaround vvv
    template<eastl::size_t N>
    ranges::subrange<char const*> c_str(char const (&sz)[N])
    {
        return {&sz[0], &sz[N-1]};
    }
#endif // EARANGES_WORKAROUND_MSVC_790554
}

void moar_tests()
{
    using namespace ranges;
    eastl::string greeting = "now is the time";
    eastl::string pattern = " ";

    {
#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        split_view sv{greeting, pattern};
#else
        split_view<views::all_t<eastl::string&>, views::all_t<eastl::string&>> sv{greeting, pattern};
#endif
        auto i = sv.begin();
        check_equal(*i, {'n','o','w'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'i','s'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'t','h','e'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'t','i','m','e'});
        ++i;
        CHECK(i == sv.end());
        //TODO: Don't work for reasons :(
        //using R = decltype(sv);
        //CPP_assert(forward_range<R>);
        //CPP_assert(forward_range<R const>);
    }

    {
#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        split_view sv{greeting, ' '};
#else
        split_view<views::all_t<eastl::string&>, single_view<char>> sv{greeting, ' '};
#endif
        auto i = sv.begin();
        CHECK(i != sv.end());
        check_equal(*i, {'n','o','w'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'i','s'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'t','h','e'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'t','i','m','e'});
        ++i;
        CHECK(i == sv.end());
        //TODO: It fails.
        //using R = decltype(sv);
        //CPP_assert(forward_range<R>);
        //CPP_assert(forward_range<R const>);
    }
/*
    {
        std::stringstream sin{greeting.c_str()};
#if EARANGES_CXX_DEDUCTION_GUIDES >= EARANGES_CXX_DEDUCTION_GUIDES_17
        auto rng = subrange{
            std::istreambuf_iterator<char>{sin},
            std::istreambuf_iterator<char>{}};
#else
        auto rng = make_subrange(
            std::istreambuf_iterator<char>{sin},
            std::istreambuf_iterator<char>{});
#endif

       auto sv = views::split(rng, ' ');
       auto i = sv.begin();
       CHECK(i != sv.end());
       check_equal(*i, {'n','o','w'});
       ++i;
       CHECK(i != sv.end());
       check_equal(*i, {'i','s'});
       ++i;
       CHECK(i != sv.end());
       check_equal(*i, {'t','h','e'});
       ++i;
       CHECK(i != sv.end());
       check_equal(*i, {'t','i','m','e'});
       ++i;
       CHECK(i == sv.end());
       
       using R = decltype(sv);
       CPP_assert(input_range<R>);
       CPP_assert(!forward_range<R>);
       CPP_assert(!input_range<R const>);
    }
*/
    {
        eastl::string list{"eggs,milk,,butter"};
        auto sv = views::split(list, ',');
        auto i = sv.begin();
        CHECK(i != sv.end());
        check_equal(*i, {'e','g','g','s'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'m','i','l','k'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, views::empty<char>);
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'b','u','t','t','e','r'});
        ++i;
        CHECK(i == sv.end());
    }
/*
    {
        eastl::string list{"eggs,milk,,butter"};
        std::stringstream sin{list.c_str()};
        auto rng = make_subrange(
            std::istreambuf_iterator<char>{sin},
            std::istreambuf_iterator<char>{});
        auto sv = rng | views::split(',');
        auto i = sv.begin();
        CHECK(i != sv.end());
        check_equal(*i, {'e','g','g','s'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'m','i','l','k'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, views::empty<char>);
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, {'b','u','t','t','e','r'});
        ++i;
        CHECK(i == sv.end());
    }
*/
    {
        eastl::string hello("hello");
        auto sv = views::split(hello, views::empty<char>);
        auto i = sv.begin();
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'h'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'e'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'o'});
        ++i;
        CHECK(i == sv.end());
    }
/*
    {
        eastl::string hello{"hello"};
        std::stringstream sin{hello.c_str()};
        auto rng = make_subrange(
            std::istreambuf_iterator<char>{sin},
            std::istreambuf_iterator<char>{});
        auto sv = views::split(rng, views::empty<char>);
        auto i = sv.begin();
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'h'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'e'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'o'});
        ++i;
        CHECK(i == sv.end());
    }
*/
    {
        eastl::string hello{"hello"};
        auto sv = views::split(hello, views::empty<char>);
        auto i = sv.begin();
        CHECK(i != sv.end());
        ++i;
        CHECK(i != sv.end());
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        ++i;
        CHECK(i == sv.end());
    }
/*
    {
        eastl::string hello{"hello"};
        std::stringstream sin{hello.c_str()};
        auto rng = make_subrange(
            std::istreambuf_iterator<char>{sin},
            std::istreambuf_iterator<char>{});
        auto sv = views::split(rng, views::empty<char>);
        auto i = sv.begin();
        CHECK(i != sv.end());
        ++i;
        CHECK(i != sv.end());
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        check_equal(*i, single_view<char>{'l'});
        ++i;
        CHECK(i != sv.end());
        ++i;
        CHECK(i == sv.end());
    }
    */
}

int main()
{
    using namespace ranges;

    {
        eastl::string str("Now is the time for all good men to come to the aid of their country.");
        auto rng = views::split(str, ' ');
        CHECK(distance(rng) == 16);
        if(distance(rng) == 16)
        {
            check_equal(*(next(begin(rng),0)), c_str("Now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
            check_equal(*(next(begin(rng),4)), c_str("for"));
            check_equal(*(next(begin(rng),5)), c_str("all"));
            check_equal(*(next(begin(rng),6)), c_str("good"));
            check_equal(*(next(begin(rng),7)), c_str("men"));
            check_equal(*(next(begin(rng),8)), c_str("to"));
            check_equal(*(next(begin(rng),9)), c_str("come"));
            check_equal(*(next(begin(rng),10)), c_str("to"));
            check_equal(*(next(begin(rng),11)), c_str("the"));
            check_equal(*(next(begin(rng),12)), c_str("aid"));
            check_equal(*(next(begin(rng),13)), c_str("of"));
            check_equal(*(next(begin(rng),14)), c_str("their"));
            check_equal(*(next(begin(rng),15)), c_str("country."));
        }
    }

    {
        eastl::string str("Now is the time for all good men to come to the aid of their country.");
        auto rng = views::split(str, c_str(" "));
        CHECK(distance(rng) == 16);
        if(distance(rng) == 16)
        {
            check_equal(*(next(begin(rng),0)), c_str("Now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
            check_equal(*(next(begin(rng),4)), c_str("for"));
            check_equal(*(next(begin(rng),5)), c_str("all"));
            check_equal(*(next(begin(rng),6)), c_str("good"));
            check_equal(*(next(begin(rng),7)), c_str("men"));
            check_equal(*(next(begin(rng),8)), c_str("to"));
            check_equal(*(next(begin(rng),9)), c_str("come"));
            check_equal(*(next(begin(rng),10)), c_str("to"));
            check_equal(*(next(begin(rng),11)), c_str("the"));
            check_equal(*(next(begin(rng),12)), c_str("aid"));
            check_equal(*(next(begin(rng),13)), c_str("of"));
            check_equal(*(next(begin(rng),14)), c_str("their"));
            check_equal(*(next(begin(rng),15)), c_str("country."));
        }
    }

    {
        eastl::string str("Now is the time for all ggood men to come to the aid of their country.");
        auto rng = views::split_when(str, starts_with_g{});
        CHECK(distance(rng) == 3);
        if(distance(rng) == 3)
        {
            check_equal(*begin(rng), c_str("Now is the time for all "));
            check_equal(*next(begin(rng)), c_str("g"));
            check_equal(*(next(begin(rng),2)), c_str("good men to come to the aid of their country."));
        }
    }

    {
        eastl::string str("Now is the time for all ggood men to come to the aid of their country.");
        ForwardIterator<eastl::string::iterator> i {str.begin()};
        auto rng = views::counted(i, str.size()) | views::split_when(starts_with_g{});
        CHECK(distance(rng) == 3);
        if(distance(rng) == 3)
        {
            check_equal(*begin(rng), c_str("Now is the time for all "));
            check_equal(*next(begin(rng)), c_str("g"));
            check_equal(*(next(begin(rng),2)), c_str("good men to come to the aid of their country."));
        }
    }

    {
        eastl::string str("meow");
        auto rng = views::split(str, views::empty<char>);
        CHECK(distance(rng) == 4);
        if(distance(rng) == 4)
        {
            check_equal(*(next(begin(rng),0)), c_str("m"));
            check_equal(*(next(begin(rng),1)), c_str("e"));
            check_equal(*(next(begin(rng),2)), c_str("o"));
            check_equal(*(next(begin(rng),3)), c_str("w"));
        }
    }

    {
        int a[] = {0, 2, 3, 1, 4, 5, 1, 6, 7};
        auto rng = a | views::remove_if([](int i) { return i % 2 == 0; });
        auto srng = views::split(rng, 1);
        CHECK(distance(srng) == 3);
        check_equal(*begin(srng), {3});
        check_equal(*next(begin(srng), 1), {5});
        check_equal(*next(begin(srng), 2), {7});
    }

    {
        eastl::string str("now  is \t the\ttime");
        auto rng = views::split_when(str, (int(*)(int))&std::isspace);
        CHECK(distance(rng) == 4);
        if(distance(rng) == 4)
        {
            check_equal(*(next(begin(rng),0)), c_str("now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
        }
    }

    {   // Regression test for https://stackoverflow.com/questions/49015671
        auto const str = "quick brown fox";
        auto rng = views::c_str(str) | views::split(' ');
        //TODO: It doesn't compile CPP_assert(forward_range<decltype(rng)>);
    }

    {   // Regression test for #986
        eastl::string s;
        s | ranges::views::split_when([](char) { return true; });
    }

    moar_tests();

    {   // Regression test for #1041
        auto is_escape = [](auto first, auto last) {
            return eastl::make_pair(next(first) != last, first);
        };

        auto escapes = views::split_when(views::c_str(R"(\t)"), is_escape);
        CPP_assert(forward_range<decltype(escapes)>);

        auto const first = begin(escapes);
        CHECK(first != end(escapes));
        CHECK(first != next(first));
    }

    return test_result();
}
