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
#include <EARanges/action/push_front.hpp>
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
        push_front(v, {1,2,3});
        ::check_equal(v, {1,2,3});

        push_front(v, views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,1,2,3});

        push_front(v, views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,10,11,12,1,2,3});

        int rg[] = {9,8,7};
        push_front(v, rg);
        ::check_equal(v, {9,8,7,10,11,12,10,11,12,1,2,3});
        push_front(v, rg);
        ::check_equal(v, {9,8,7,9,8,7,10,11,12,10,11,12,1,2,3});

        eastl::list<int> s;
        push_front(s, views::ints|views::take(10)|views::for_each([](int i){return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        push_front(s, -2);
        ::check_equal(s, {-2,0,2,4,6,8});
    }

    {
        eastl::vector<int> v;
        v = eastl::move(v) | push_front({1,2,3});
        ::check_equal(v, {1,2,3});

        v = eastl::move(v) | push_front(views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,1,2,3});

        v = eastl::move(v) | push_front(views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,10,11,12,1,2,3});

        int rg[] = {9,8,7};
        v = eastl::move(v) | push_front(rg);
        ::check_equal(v, {9,8,7,10,11,12,10,11,12,1,2,3});
        v = eastl::move(v) | push_front(rg);
        ::check_equal(v, {9,8,7,9,8,7,10,11,12,10,11,12,1,2,3});

        eastl::list<int> s;
        s = eastl::move(s) | push_front(views::ints|views::take(10)|views::for_each([](int i){return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        s = eastl::move(s) | push_front(-2);
        ::check_equal(s, {-2,0,2,4,6,8});
    }

    {
        eastl::vector<int> v;
        v |= push_front({1,2,3});
        ::check_equal(v, {1,2,3});

        v |= push_front(views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,1,2,3});

        v |= push_front(views::iota(10) | views::take(3));
        ::check_equal(v, {10,11,12,10,11,12,1,2,3});

        int rg[] = {9,8,7};
        v |= push_front(rg);
        ::check_equal(v, {9,8,7,10,11,12,10,11,12,1,2,3});
        v |= push_front(rg);
        ::check_equal(v, {9,8,7,9,8,7,10,11,12,10,11,12,1,2,3});

        eastl::list<int> s;
        s |= push_front(views::ints|views::take(10)|views::for_each([](int i){return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        s |= push_front(-2);
        ::check_equal(s, {-2,0,2,4,6,8});
    }

    return ::test_result();
}
