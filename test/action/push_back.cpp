// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/action/push_back.hpp>
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
        eastl::vector<int> v;
        push_back(v, {1,2,3});
        ::check_equal(v, {1,2,3});

        push_back(v, views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12});

        push_back(v, views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12,10,11,12});

        int rg[] = {9,8,7};
        push_back(v, rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7});
        push_back(v, rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7,9,8,7});

        eastl::list<int> s;
        push_back(s, views::ints|views::take(10) | views::for_each([](int i) { return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        push_back(s, 10);
        ::check_equal(s, {0,2,4,6,8,10});
    }

    {
        eastl::vector<int> v;
        v = eastl::move(v) | push_back({1,2,3});
        ::check_equal(v, {1,2,3});

        v = eastl::move(v) | push_back(views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12});

        v = eastl::move(v) | push_back(views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12,10,11,12});

        int rg[] = {9,8,7};
        v = eastl::move(v) | push_back(rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7});
        v = eastl::move(v) | push_back(rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7,9,8,7});

        eastl::list<int> s;
        s = eastl::move(s) | push_back(views::ints|views::take(10)|views::for_each([](int i){return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        s = eastl::move(s) | push_back(10);
        ::check_equal(s, {0,2,4,6,8,10});
    }

    {
        eastl::vector<int> v;
        v |= push_back({1,2,3});
        ::check_equal(v, {1,2,3});

        v |= push_back(views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12});

        v |= push_back(views::iota(10) | views::take(3));
        ::check_equal(v, {1,2,3,10,11,12,10,11,12});

        int rg[] = {9,8,7};
        v |= push_back(rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7});
        v |= push_back(rg);
        ::check_equal(v, {1,2,3,10,11,12,10,11,12,9,8,7,9,8,7});

        eastl::list<int> s;
        s |= push_back(views::ints|views::take(10) | views::for_each([](int i) { return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        s |= push_back(10);
        ::check_equal(s, {0,2,4,6,8,10});
    }

    return ::test_result();
}
