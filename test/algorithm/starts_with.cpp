// Range v3 library
//
//  Copyright 2019 Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
#include <EARanges/algorithm/starts_with.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include <EASTL/slist.h>
#include <EARanges/range/conversion.hpp>
#include <EARanges/view/iota.hpp>
#include <EARanges/view/slice.hpp>
#include <EARanges/view/take_exactly.hpp>
#include <EARanges/view/istream.hpp>
#include <sstream>
#include <EASTL/vector.h>
#include "../eastl_utils.h"

void test_defaults()
{
   using namespace ranges;
   { // checks starts_with works for input ranges
      constexpr auto full_latin_alphabet = "a b c d e f g h i j k l m n o p q r s t u v w x y z";
      auto const partial_latin_alphabet = "a b c d";

      {
         auto long_stream = std::istringstream{full_latin_alphabet};
         auto short_stream = std::istringstream{partial_latin_alphabet};

         auto r1 = istream<char>(long_stream);
         auto r2 = istream<char>(short_stream);
         CHECK(starts_with(begin(r1), end(r1), begin(r2), end(r2)));
      }
      {
         auto long_stream = std::istringstream{full_latin_alphabet};
         auto short_stream = std::istringstream{partial_latin_alphabet};

         auto r1 = istream<char>(long_stream);
         auto r2 = istream<char>(short_stream);
         CHECK(!starts_with(begin(r2), end(r2), begin(r1), end(r1)));
      }
      {
         auto long_stream = std::istringstream{full_latin_alphabet};
         auto short_stream = std::istringstream{partial_latin_alphabet};
         CHECK(starts_with(istream<char>(long_stream), istream<char>(short_stream)));
      }
      {
         auto long_stream = std::istringstream{full_latin_alphabet};
         auto short_stream = std::istringstream{partial_latin_alphabet};
         CHECK(!starts_with(istream<char>(short_stream), istream<char>(long_stream)));
      }
   }
   { // checks starts_with works for random-access ranges
#ifdef EARANGES_WORKAROUND_MSVC_779708
      auto const long_range = views::iota(0, 100) | to<eastl::vector>();
      auto const short_range = views::iota(0, 10) | to<eastl::vector>();
#else // ^^^ workaround / no workaround vvv
      auto const long_range = views::iota(0, 100) | to<eastl::vector>;
      auto const short_range = views::iota(0, 10) | to<eastl::vector>;
#endif // EARANGES_WORKAROUND_MSVC_779708

      CHECK(starts_with(begin(long_range), end(long_range), begin(short_range), end(short_range)));
      CHECK(starts_with(long_range, short_range));

      CHECK(!starts_with(begin(short_range), end(short_range), begin(long_range), end(long_range)));
      CHECK(!starts_with(short_range, long_range));
   }
   { // checks starts_with works for random-access ranges with arbitrary sentinels
      auto const long_range = views::iota(0);
      auto const short_range = views::iota(0) | views::take_exactly(100);

      CHECK(starts_with(begin(long_range), end(long_range), begin(short_range), end(short_range)));
      CHECK(starts_with(long_range, short_range));

      CHECK(!starts_with(begin(short_range), end(short_range), begin(long_range), end(long_range)));
      CHECK(!starts_with(short_range, long_range));
   }
   { // checks starts_with identifies a subrange
      auto const range = views::iota(0) | views::slice(50, 100);
      auto const offset = views::iota(50, 100);

      CHECK(starts_with(begin(range), end(range), begin(offset), end(offset)));
      CHECK(starts_with(range, offset));

      CHECK(starts_with(begin(offset), end(offset), begin(range), end(range)));
      CHECK(starts_with(offset, range));
   }
   { // checks starts_with identifies when two ranges don't have the same start sequence
      auto const first = views::iota(0, 1'000);
      auto const second = views::iota(10, 1'000);

      CHECK(!starts_with(begin(first), end(first), begin(second), end(second)));
      CHECK(!starts_with(first, second));

      CHECK(!starts_with(begin(second), end(second), begin(first), end(first)));
      CHECK(!starts_with(second, first));
   }
}

void test_comparison()
{
   using namespace ranges;
   auto const long_range = views::iota(0, 100);
   auto const short_range = views::iota(1, 51);
   CHECK(starts_with(begin(long_range), end(long_range), begin(short_range), end(short_range), less{}));
   CHECK(starts_with(long_range, short_range, less{}));

   CHECK(!starts_with(begin(long_range), end(long_range), begin(short_range), end(short_range), greater{}));
   CHECK(!starts_with(long_range, short_range, greater{}));
}

int main()
{
   ::test_defaults();
   ::test_comparison();
   return ::test_result();
}
