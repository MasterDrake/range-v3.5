// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <iostream>
#include <EASTL/tuple.h>
#include <EASTL/memory.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <complex>
#include <EARanges/concepts/concepts.hpp>
#include <EARanges/utility/swap.hpp>
#include <EARanges/view/zip.hpp>
#include <EARanges/range/conversion.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

//TODO:27) No swap for as because of linking error eastl::swap :/. Basically in ranges-v3 it works because the ADL searches for an user defined swap for movable only objects
//which is found in <memory> for unique_ptr, so it uses that. Here it should realize that there is no such thing and user other overloads :/. I guess that has more priority
//Adding a user defined swap helps

namespace eastl
{
    //inline void swap(MoveOnlyString & a, MoveOnlyString& b)
    //{
    //    MoveOnlyString&& tmp = eastl::move(a);
    //    a = eastl::move(b);
    //    b = eastl::move(tmp);
    //}
    //template<typename T>
    //inline void swap(T&& a, T&& b) noexcept((eastl::is_nothrow_move_constructible<T>::value&&
    //        eastl::is_nothrow_move_assignable<T>::value))
    //{
    //    T && tmp = eastl::move(a);
    //    a = eastl::move(b);
    //    b = eastl::move(tmp);
    //}
 
} // namespace eastl
namespace eastl
{
    inline void swap(MoveOnlyString & a, MoveOnlyString & b) noexcept
    {
        MoveOnlyString && tmp = eastl::move(a);
        a = eastl::move(b);
        b = eastl::move(tmp);
    }
}

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

template<typename T>
struct S
{
    T t;
};

int main()
{
    int a=0,b=42;
    ranges::swap(a,b);
    CHECK(a == 42);
    CHECK(b == 0);
    //TODO:26)This static assert fails, it's a problem of eastl::pair?? More like with the fact that they're rvalues
    CPP_assert(!ranges::swappable_with<eastl::pair<int,int>&&, eastl::pair<int,int>&&>);
    CPP_assert(ranges::swappable_with<eastl::pair<int&,int&>&&,eastl::pair<int&,int&>&&>);

    int c=24,d=82;
    ranges::swap(eastl::tie(a,b),eastl::tie(c,d));
    CHECK(a == 24);
    CHECK(b == 82);
    CHECK(c == 42);
    CHECK(d == 0);

    // Swap pairs of tuple proxies
    int e=1,f=2,g=3,h=4;
    ranges::swap(eastl::make_pair(eastl::tie(a,b), eastl::tie(c,d)), eastl::make_pair(eastl::tie(e,f), eastl::tie(g,h)));
    CHECK(a == 1);
    CHECK(b == 2);
    CHECK(c == 3);
    CHECK(d == 4);
    CHECK(e == 24);
    CHECK(f == 82);
    CHECK(g == 42);
    CHECK(h == 0);

#ifndef _LIBCPP_VERSION
    // Swap tuples of pair proxies
    ranges::swap(eastl::make_tuple(eastl::make_pair(eastl::ref(a),eastl::ref(b)), eastl::make_pair(eastl::ref(c),eastl::ref(d))),
                 eastl::make_tuple(eastl::make_pair(eastl::ref(e),eastl::ref(f)), eastl::make_pair(eastl::ref(g),eastl::ref(h))));
    CHECK(a == 24);
    CHECK(b == 82);
    CHECK(c == 42);
    CHECK(d == 0);
    CHECK(e == 1);
    CHECK(f == 2);
    CHECK(g == 3);
    CHECK(h == 4);
#endif

    //int aa=24,bb=82;
    //ranges::iter_swap(&aa, &bb);
    //CHECK(aa == 82);
    //CHECK(bb == 24);
    //
    //eastl::unique_ptr<int> u0{new int{1}};
    //eastl::unique_ptr<int> u1{new int{2}};
    //int *p0 = u0.get();
    //int *p1 = u1.get();
    //ranges::iter_swap(&u0, &u1);
    //CHECK(u0.get() == p1);
    //CHECK(u1.get() == p0);

    {
        using namespace ranges;
        using eastl::swap;
        static_assert(eastl::is_swappable_v<MoveOnlyString>, "MoveOnlyString should be swappable");
        static_assert(eastl::is_swappable_v<eastl::string>, "eastl::string should be swappable");

        auto v0 = to<eastl::vector<eastl::string>>({"a","b","c"});
        auto v1 = to<eastl::vector<eastl::string>>({"x","y","z"});
        auto rng = views::zip(v0, v1);
        ranges::iter_swap(rng.begin(), rng.begin()+2);
        ::check_equal(v0, {"c","b","a"});
        ::check_equal(v1, {"z","y","x"});
    }

    {
        using T = std::complex<float>;
        T s,t;
        ranges::swap(s,t);
    }
    {
        using T = S<std::complex<float>>;
        T s,t;
        ranges::swap(s,t);
    }

    return ::test_result();
}
