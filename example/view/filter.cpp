//! [filter example]
#include <iostream>
#include <EASTL/vector.h>
#include <EARanges/view/filter.hpp>

#include "../test/eastl_utils.h"

int main()
{
    eastl::vector<int> numbers{1, 2, 3, 4};

    auto even = numbers
        // Keep only the even numbers
        | ranges::views::filter([](const int& num) 
        {
          return num % 2 == 0;
        });

    std::cout << even << '\n';
}
//! [filter example]