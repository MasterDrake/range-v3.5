// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <cctype>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include <EARanges/action/split.hpp>
#include <EARanges/action/split_when.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/core.hpp>
#include <EARanges/view/c_str.hpp>
#include <EARanges/view/iota.hpp>

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

    {
        auto v = views::ints(1, 21) | to<eastl::vector>();
        eastl::vector<eastl::vector<int>> rgv = actions::split(v, 10);
        CHECK(rgv.size() == 2u);
        ::check_equal(rgv[0], {1,2,3,4,5,6,7,8,9});
        ::check_equal(rgv[1], {11,12,13,14,15,16,17,18,19,20});

        using I = eastl::vector<int>::iterator;
        eastl::vector<eastl::vector<int>> rgv2 = actions::split_when(
            v, [](I b, I) { return eastl::make_pair(0 == (*b) % 2, next(b)); });
        CHECK(rgv2.size() == 10u);
        ::check_equal(rgv2[0], {1});
        ::check_equal(rgv2[1], {3});
        ::check_equal(rgv2[2], {5});
        ::check_equal(rgv2[3], {7});
        ::check_equal(rgv2[4], {9});
        ::check_equal(rgv2[5], {11});
        ::check_equal(rgv2[6], {13});
        ::check_equal(rgv2[7], {15});
        ::check_equal(rgv2[8], {17});
        ::check_equal(rgv2[9], {19});
    }
    //TODO:3a) Typical eastl::string and lack of Char traits, c_str needs a specialization only for eastl::string
    //{
        eastl::string s{"This is his face"};
        eastl::vector<eastl::string> rgs = actions::split(s, views::c_str(" "));
        CHECK(rgs.size() == 4u);
        CHECK(rgs[0] == "This");
        CHECK(rgs[1] == "is");
        CHECK(rgs[2] == "his");
        CHECK(rgs[3] == "face");
    //}

    //{
    //    eastl::string s{"This is his face"};
    //    eastl::vector<eastl::string> rgs = eastl::move(s) | actions::split(views::c_str(" "));
    //    CHECK(rgs.size() == 4u);
    //    CHECK(rgs[0] == "This");
    //    CHECK(rgs[1] == "is");
    //    CHECK(rgs[2] == "his");
    //    CHECK(rgs[3] == "face");
    //}

    //{
    //    eastl::string s{"This is his face"};
    //    char ch[] =  {' '};
    //    eastl::vector<eastl::string> rgs = actions::split(s, ch);
    //    CHECK(rgs.size() == 4u);
    //    CHECK(rgs[0] == "This");
    //    CHECK(rgs[1] == "is");
    //    CHECK(rgs[2] == "his");
    //    CHECK(rgs[3] == "face");
    //}

    //{
    //    eastl::string s{"This is his face"};
    //    char ch[] =  {' '};
    //    eastl::vector<eastl::string> rgs = eastl::move(s) | actions::split(ch);
    //    CHECK(rgs.size() == 4u);
    //    CHECK(rgs[0] == "This");
    //    CHECK(rgs[1] == "is");
    //    CHECK(rgs[2] == "his");
    //    CHECK(rgs[3] == "face");
    //}

    {
        auto rgi = views::ints(1,21);
        eastl::vector<eastl::vector<int>> rgv3 = actions::split(rgi, 10);
        CHECK(rgv3.size() == 2u);
        ::check_equal(rgv3[0], {1,2,3,4,5,6,7,8,9});
        ::check_equal(rgv3[1], {11,12,13,14,15,16,17,18,19,20});
    }

    {
        auto rgi = views::ints(1,21);
        eastl::vector<eastl::vector<int>> rgv3 = eastl::move(rgi) | actions::split(10);
        CHECK(rgv3.size() == 2u);
        ::check_equal(rgv3[0], {1,2,3,4,5,6,7,8,9});
        ::check_equal(rgv3[1], {11,12,13,14,15,16,17,18,19,20});
    }
    // TODO:3b) Cannot convert char to eastl::string
    {
        //eastl::string str("now  is \t the\ttime");
        //auto toks = actions::split_when(str, +[](int i) { return std::isspace(i); });
       // static_assert(eastl::is_same<decltype(toks), eastl::vector<eastl::string>>::value, "");
        //CHECK(toks.size() == 4u);
        //if(toks.size() == 4u)
       // {
            //CHECK(toks[0] == "now");
            //CHECK(toks[1] == "is");
            //CHECK(toks[2] == "the");
            //CHECK(toks[3] == "time");
       // }
    }
    // TODO:3c) Cannot convert char to eastl::string
    {
       // eastl::string str("now  is \t the\ttime");
       // auto toks =  eastl::move(str) | actions::split_when(+[](int i) { return std::isspace(i); });
       // static_assert(eastl::is_same<decltype(toks), eastl::vector<eastl::string>>::value, "");
      //  CHECK(toks.size() == 4u);
       // if(toks.size() == 4u)
      //  {
            //CHECK(toks[0] == "now");
            //CHECK(toks[1] == "is");
            //CHECK(toks[2] == "the");
            //CHECK(toks[3] == "time");
      //  }
    }

    return ::test_result();
}
