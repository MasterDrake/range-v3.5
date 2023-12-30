#include <EASTL/vector.h>
#include <EARanges/action/sort.hpp>

template<typename A, typename B>
constexpr auto operator-(A a, B)
{
    return a;
}

int main()
{
    eastl::vector<int> data{};
    data |= ranges::actions::sort;
}
