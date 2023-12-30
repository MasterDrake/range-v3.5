// Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <EARanges/core.hpp>
#include <EARanges/algorithm/none_of.hpp>
#include "../simple_test.hpp"

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

constexpr bool even(int n) { return n % 2 == 0; }

struct S {
  S(bool p) : test(p) { }

  bool p() const { return test; }

  bool test;
};

constexpr bool test_constexpr(std::initializer_list<int> il)
{
    return ranges::none_of(il, even);
}

int main()
{
  eastl::vector<int> all_even { 0, 2, 4, 6 };
  eastl::vector<int> one_even { 1, 3, 4, 7 };
  eastl::vector<int> none_even { 1, 3, 5, 7 };
  CHECK(!ranges::none_of(all_even.begin(), all_even.end(), even));
  CHECK(!ranges::none_of(one_even.begin(), one_even.end(), even));
  CHECK(ranges::none_of(none_even.begin(), none_even.end(), even));

  CHECK(!ranges::none_of(all_even, even));
  CHECK(!ranges::none_of(one_even, even));
  CHECK(ranges::none_of(none_even, even));

  using ILI = std::initializer_list<int>;
  CHECK(!ranges::none_of(ILI{0, 2, 4, 6}, [](int n) { return n % 2 == 0; }));
  CHECK(!ranges::none_of(ILI{1, 3, 4, 7}, [](int n) { return n % 2 == 0; }));
  CHECK(ranges::none_of(ILI{1, 3, 5, 7}, [](int n) { return n % 2 == 0; }));

  eastl::vector<S> all_true { true, true, true };
  eastl::vector<S> one_true { false, false, true };
  eastl::vector<S> none_true { false, false, false };
  CHECK(!ranges::none_of(all_true.begin(), all_true.end(), &S::p));
  CHECK(!ranges::none_of(one_true.begin(), one_true.end(), &S::p));
  CHECK(ranges::none_of(none_true.begin(), none_true.end(), &S::p));

  CHECK(!ranges::none_of(all_true, &S::p));
  CHECK(!ranges::none_of(one_true, &S::p));
  CHECK(ranges::none_of(none_true, &S::p));

  using ILS = std::initializer_list<S>;
  CHECK(!ranges::none_of(ILS{S(true), S(true), S(true)}, &S::p));
  CHECK(!ranges::none_of(ILS{S(false), S(true), S(false)}, &S::p));
  CHECK(ranges::none_of(ILS{S(false), S(false), S(false)}, &S::p));

  STATIC_CHECK(!test_constexpr({0, 2, 4, 6}));
  STATIC_CHECK(!test_constexpr({1, 3, 4, 7}));
  STATIC_CHECK(test_constexpr({1, 3, 5, 7}));

  return ::test_result();
}
