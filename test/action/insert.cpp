// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <EASTL/set.h>
#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/take.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/ref.hpp>
#include <EARanges/action/insert.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

template<typename T>
struct vector_like : eastl::vector<T>
{
    using eastl::vector<T>::vector;

    using typename eastl::vector<T>::size_type;
    using typename eastl::vector<T>::value_type;

    size_type last_reservation{};
    size_type reservation_count{};

    inline T* DoAllocate(size_type n)
    {
      last_reservation = n;
      ++reservation_count;
      return eastl::vector<T>::DoAllocate(n);
    }
};

int main()
{
    using namespace ranges;

    {
        eastl::vector<int> v;
        auto i = insert(v, v.begin(), 42);
        CHECK(i == v.begin());
        ::check_equal(v, {42});
        insert(v, v.end(), {1,2,3});
        ::check_equal(v, {42,1,2,3});

        insert(v, v.begin(), views::ints | views::take(3));
        ::check_equal(v, {0,1,2,42,1,2,3});

        int rg[] = {9,8,7};
        insert(v, v.begin()+3, rg);
        ::check_equal(v, {0,1,2,9,8,7,42,1,2,3});
        insert(v, v.begin()+1, rg);
        ::check_equal(v, {0,9,8,7,1,2,9,8,7,42,1,2,3});
    }

    {
        eastl::set<int> s;
        insert(s, views::ints|views::take(10) | views::for_each([](int i){return yield_if(i%2==0,i);}));
        ::check_equal(s, {0,2,4,6,8});
        auto j = insert(s, 10);
        CHECK(j.first == prev(s.end()));
        CHECK(j.second == true);
        ::check_equal(s, {0,2,4,6,8,10});

        insert(views::ref(s), 12);
        ::check_equal(s, {0,2,4,6,8,10,12});
    }
    //TODO:1) No allocation is being made for this vector_like :/ I can't override DoAllocate which is the function called in insert 
    // 1162
    // vector<T, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last)

    /*{
       const eastl::size_t N = 1024;
       vector_like<int> vl;
       insert(vl, vl.end(), views::iota(0, int{N}));
       CHECK(vl.reservation_count == 1u);
       CHECK(vl.last_reservation == N);
       auto r = views::iota(0, int{2 * N});
       insert(vl, vl.begin() + 42, begin(r), end(r));
       CHECK(vl.reservation_count == 2u);
       CHECK(vl.last_reservation == 3 * N);
       int i = 42;
       insert(vl, vl.end(), &i, &i + 1);
       CHECK(vl.reservation_count == 3u);
       CHECK(vl.last_reservation > 3 * N + 1);
    }*/

    return ::test_result();
}
