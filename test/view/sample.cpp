#include <EASTL/ranges/view/sample.hpp>
#include <EASTL/ranges/algorithm/equal.hpp>
#include <EASTL/numeric.h>
#include <EASTL/vector.h>
//TODO:39) random and uniform_int_distribution shenaningans ...
#include <random>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

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

int main ()
{
    std::mt19937 engine;

    eastl::vector<int> pop(100);
    eastl::iota(eastl::begin(pop), eastl::end(pop), 0);
    {
        constexpr int N = 32;
        eastl::array<int, N> tmp;
        auto rng = pop | views::sample(N, engine);
        using Rng = decltype(rng);
        CPP_assert(input_range<Rng> && view_<Rng>);
        CPP_assert(!forward_range<Rng>);
        ranges::copy(rng, tmp.begin());
        rng = pop | views::sample(N, engine);
        CHECK(!ranges::equal(rng, tmp));
        engine = decltype(engine){};
        rng = pop | views::sample(N, engine);
        CHECK(ranges::equal(rng, tmp));
    }

    {
        int const some_ints[] = {0,1,2,3,4,5,6,7,8};
        auto rng = debug_input_view<int const>{some_ints} | views::sample(4, engine);
        using Rng = decltype(rng);
        CPP_assert(input_range<Rng> && view_<Rng>);
        CPP_assert(!forward_range<Rng>);
        CHECK(ranges::distance(rng) == 4);
    }

    return ::test_result();
}
