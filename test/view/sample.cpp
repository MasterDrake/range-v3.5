#include <EARanges/view/sample.hpp>
#include <EARanges/algorithm/equal.hpp>
#include <EASTL/numeric.h>
#include <EASTL/vector.h>
#include <EASTL/random.h>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

int main ()
{
    eastl::default_random_engine engine;

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
