// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Google LLC 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
#include <sstream>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include <EARanges/algorithm/copy.hpp>
#include <EARanges/iterator/stream_iterators.hpp>
#include <EARanges/range/conversion.hpp>
#include <EARanges/view/for_each.hpp>
#include <EARanges/view/reverse.hpp>
#include <EARanges/view/sliding.hpp>
#include <EARanges/view/stride.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

//TODO:21) technically this doesn't work but eastl doesn't have streams capabilities so that's why this can be skipped or ""easily"" fixed by implementing such facilities. Just skip it or remove it.
//IN realt� � sempre causato dal fatto che string non accetta un paio di iterators...
namespace
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    template<typename>
    eastl::string endian_adjust(eastl::string const & s)
    {
        return s;
    }
#else
    template<typename T>
    eastl::string endian_adjust(eastl::string const & s)
    {
        namespace rv = ranges::views;
        return eastl::string();
        return rv::sliding(s, static_cast<std::ptrdiff_t>(sizeof(T))) //
               | rv::stride(static_cast<std::ptrdiff_t>(sizeof(T)))   //
               | rv::for_each([](auto x) { return x | rv::reverse; }) //
               | ranges::to<eastl::string>;
    }
#endif // endianness
} // namespace

int main()
{
    {
        constexpr auto expected = "\x21\x22\x23\x24";

        {
            auto const input = eastl::vector<char>{0x21, 0x22, 0x23, 0x24};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = output.str();
            CHECK(actual == expected);
        }
        {
            auto const input = eastl::vector<unsigned short>{0x2122, 0x2324};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = endian_adjust<unsigned short>(output.str().c_str());
            CHECK(actual == expected);
        }
#if __cplusplus > 201703L
        {
            // float computed to be *exactly* 0x21222324.
            auto const input = eastl::vector<float>{0x1.4446480000000000179Dp-61f};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = endian_adjust<float>(output.str().c_str());
            CHECK(actual == expected);
        }
#endif // __cplusplus > 201703L
    }
    {
        constexpr auto expected = "\x21\x22\x23\x24\x25\x26\x27\x28";
        {
            auto const input = eastl::vector<unsigned int>{0x21222324, 0x25262728};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = endian_adjust<unsigned int>(output.str().c_str());
            CHECK(actual == expected);
        }
#if __cplusplus > 201703L
        {
            // floats computed to be *exactly* 0x21222324 and 0x25262728.
            auto const input =
                eastl::vector<float>{0x1.4446480000000000179Dp-61f, 0x1.4C4E5p-53f};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = endian_adjust<unsigned int>(output.str().c_str());
            CHECK(actual == expected);
        }
        {
            // double computed to be *exactly* 0x2122232425262728.
            auto const input = eastl::vector<double>{0x1.223242526272800006F2p-493};
            auto output = std::ostringstream();
            ranges::copy(input, ranges::unformatted_ostream_iterator<>(output));

            auto const actual = endian_adjust<double>(output.str().c_str());
            CHECK(actual == expected);
        }
#endif // __cplusplus > 201703L
    }
    return ::test_result();
}
