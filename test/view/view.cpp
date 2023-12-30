#include <EASTL/vector.h>

#include <EARanges/view/drop.hpp>
#include <EARanges/view/view.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

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

using namespace ranges;

struct my_drop_base_fn
{
    template<typename Rng>
    auto operator()(Rng && rng, range_difference_t<Rng> n) const
    {
        return drop_view<views::all_t<Rng>>(views::all(static_cast<Rng &&>(rng)), n);
    }
};
struct my_drop_fn : my_drop_base_fn
{
    using my_drop_base_fn::operator();

    template<typename Int>
    constexpr auto operator()(Int n) const
    {
        return make_view_closure([=](auto && rng) {return my_drop_base_fn{}(eastl::forward<decltype(rng)>(rng), n); });
    }
};
EARANGES_INLINE_VARIABLE(my_drop_fn, my_drop)

/// #https://github.com/ericniebler/range-v3/issues/1169
void constexpr_test_1169()
{
#if EARANGES_CXX_CONSTEXPR >= EARANGES_CXX_CONSTEXPR_LAMBDAS
    constexpr auto const drop1 = my_drop(1);
    constexpr auto const drop3 = drop1 | my_drop(2);

    eastl::vector<int> vec = {1, 2, 3, 4};
    check_equal(vec | drop3, {4});
#endif
    (void)my_drop;
}

int main()
{
    constexpr_test_1169();
    return test_result();
}
