// Range v3 library
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <cstddef>
#include <EASTL/iterator.h>
#include <EARanges/iterator.hpp>

struct X { };

namespace eastl
{
    template<> struct iterator_traits<X> { };
}

struct Y {
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = int*;
    using reference = int&;
    using iterator_category = eastl::forward_iterator_tag;
};

//TODO:45) I don't like this is_std_iterator_traits thing and I can't seem to fix it unfortunately. This doesn't work.
static_assert(ranges::detail::is_std_iterator_traits_specialized_v<X>, "");
static_assert(!ranges::detail::is_std_iterator_traits_specialized_v<Y>, "");
static_assert(!ranges::detail::is_std_iterator_traits_specialized_v<int*>, "");

int main()
{
}
