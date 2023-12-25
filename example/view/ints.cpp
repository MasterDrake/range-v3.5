//! [ints example]
#include <iostream>
#include <EASTL/vector.h>
#include <EASTL/ranges/view/iota.hpp>

int main()
{
    auto numbers = ranges::views::ints(3, 7);

    std::cout << numbers << '\n';
}
//! [ints example]
