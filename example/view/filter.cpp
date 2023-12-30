//! [filter example]
#include <iostream>
#include <EASTL/vector.h>
#include <EARanges/view/filter.hpp>

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
    eastl::vector<int> numbers{1, 2, 3, 4};

    auto even = numbers
        // Keep only the even numbers
        | ranges::views::filter([](const int& num) {
          return num % 2 == 0;
        });

    std::cout << even << '\n';
}
//! [filter example]
