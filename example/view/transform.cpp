//! [transform example]
#include <EASTL/vector.h>
#include <iostream>

#include <EARanges/view/transform.hpp>

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
int main()
{
    eastl::vector<int> numbers{1, 2, 3};

    auto halved = numbers
                  // Divide each integer by 2, converting it into a double
                  | ranges::views::transform([](const int& num) { return num / 2.0; });

    std::cout << halved << '\n';
}
//! [transform example]
