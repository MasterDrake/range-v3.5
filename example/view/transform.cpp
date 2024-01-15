//! [transform example]
#include <EASTL/vector.h>
#include <iostream>

#include <EARanges/view/transform.hpp>

#include "../test/eastl_utils.h"
//TODO: vector operator overload. Maybe in eastl_utils.h
int main()
{
    eastl::vector<int> numbers{1, 2, 3};

    auto halved = numbers
                  // Divide each integer by 2, converting it into a double
                  | ranges::views::transform([](const int& num) { return num / 2.0; });

    //std::cout << halved << '\n';
}
//! [transform example]
