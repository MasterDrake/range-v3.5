// Range v3 library
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <EASTL/ranges/view/any_view.hpp>
#include <EASTL/ranges/algorithm/for_each.hpp>

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


struct Foo
{
    Foo() = default;
    Foo(Foo const&) = default;
    virtual ~Foo() = default;
    virtual void foo() = 0;
};

struct Bar : public Foo
{
    virtual void foo() override {}
};

int main()
{
    eastl::vector<Bar> bars { Bar() };
    ranges::any_view<Foo &> foos = bars;
    ranges::for_each(foos, [] (Foo & foo)
    {
        foo.foo();
    });
}
