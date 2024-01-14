// Range v3 library
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <EARanges/view/any_view.hpp>
#include <EARanges/algorithm/for_each.hpp>
#include "eastl_utils.h"


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
