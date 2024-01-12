//! [ints example]
#include <iostream>
#include <EASTL/vector.h>
#include <EARanges/view/iota.hpp>

int main()
{
    auto numbers = ranges::views::ints(3, 785);

    std::cout << numbers << '\n';
}
//! [ints example]
