/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <sstream>
#include <EASTL/vector.h>

#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/addressof.hpp>
#include <EASTL/ranges/view/facade.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/take.hpp>

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

using namespace ranges;

void simple_test()
{
    eastl::vector<int> list = {1,2,3};

    auto out = list | views::addressof;

    check_equal(out, {&list[0], &list[1], &list[2]});
}

struct test_istream_range
  : view_facade<test_istream_range, unknown>
{
private:
    friend range_access;

    eastl::vector<int> *list;

    struct cursor
    {
    private:
        std::size_t i = 0;
        eastl::vector<int> *list = nullptr;
    public:
        cursor() = default;
        explicit cursor(eastl::vector<int> &list_)
          : list(&list_)
        {}
        void next()
        {
            ++i;
        }
        int &read() const noexcept
        {
            return (*list)[i];
        }
        bool equal(default_sentinel_t) const
        {
            return i == list->size();
        }
    };

    cursor begin_cursor()
    {
        return cursor{*list};
    }
public:
    test_istream_range() = default;
    explicit test_istream_range(eastl::vector<int> &list_)
      : list(&list_)
    {}
};

void test_input_range()
{
    // It is implementation dependent,
    // for how long returned reference remains valid.
    // It should be valid at least until next read.
    // For test purposes we use custom input range.

    eastl::vector<int> list{1, 2, 3};
    auto rng = test_istream_range(list);
    CPP_assert(input_range<decltype(rng)>);

    auto out = rng | views::addressof;
    check_equal(out, {&list[0], &list[1], &list[2]});
}

struct test_xvalue_range
  : view_facade<test_xvalue_range, unknown>
{
private:
    friend range_access;

    struct cursor
    {
        cursor() = default;
        void next();
        int &&read() const noexcept;
        bool equal(default_sentinel_t) const;
    };

    cursor begin_cursor();
};

template<typename, typename = void>
constexpr bool can_view = false;
template<typename R>
constexpr bool can_view<R,
    meta::void_<decltype(views::addressof(eastl::declval<R>()))>> = true;

// prvalue ranges cannot be passed to views::addressof
CPP_assert(!can_view<decltype(views::iota(0, 3))>);
// xvalue ranges cannot be passed to views::addressof
CPP_assert(!can_view<test_xvalue_range>);

int main()
{
    simple_test();
    test_input_range();

    return test_result();
}
