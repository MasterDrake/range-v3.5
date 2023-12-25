// Range v3 library
//
//  Copyright Tobias Mayer 2016
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/slist.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/view/chunk_by.hpp>
#include <EASTL/ranges/view/cycle.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/repeat.hpp>
#include <EASTL/ranges/view/repeat_n.hpp>
#include <EASTL/ranges/view/reverse.hpp>
#include <EASTL/ranges/view/sliding.hpp>
#include <EASTL/ranges/view/zip.hpp>
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

namespace
{
    namespace ebo_test
    {
        struct empty1 {};
        struct empty2 {};
        struct refines : empty1, empty2 {};
    }
    constexpr bool broken_ebo = sizeof(ebo_test::refines) > sizeof(ebo_test::empty1);

    constexpr auto N = 7;
    constexpr auto K = 3;
    CPP_assert(K < N);

    template<typename Adapted>
    void test_size(Adapted& a, eastl::true_type)
    {
        CPP_assert(sized_range<decltype(a)>);
        CHECK(size(a), unsigned(N - K + 1));
    }
    template<typename Adapted>
    void test_size(Adapted&, eastl::false_type)
    {}

    template<typename Adapted>
    void test_common(Adapted& a, eastl::true_type)
    {
        CPP_assert(common_range<decltype(a)>);
    }
    // template<typename Adapted>
    // void test_common(Adapted&, eastl::false_type)
    // {}

    template<typename Adapted>
    void test_prev(Adapted& a, iterator_t<Adapted> const& it, eastl::true_type)
    {
        CPP_assert(bidirectional_range<decltype(a)>);
        ::check_equal(*ranges::prev(it, 3), views::iota(N - K - 2, N - 2));
    }
    template<typename Adapted>
    void test_prev(Adapted&, iterator_t<Adapted> const&, eastl::false_type)
    {}

    template<typename Rng, bool>
    struct size_compare_
    {
        iterator_t<Rng> iter1_;
        iterator_t<Rng> iter2_;
        range_difference_t<Rng> dist_;
    };
    template<typename Rng>
    struct size_compare_<Rng, true>
    {
        iterator_t<Rng> iter1_;
        range_difference_t<Rng> dist_;
    };

    template<typename Rng>
    using size_compare =
        size_compare_<Rng, random_access_range<Rng> || (bidirectional_range<Rng> && common_range<Rng>)>;

    template<typename ToBase>
    void test_finite(ToBase to_base)
    {
        auto v = to_base(views::iota(0,N));
        using Base = decltype(v);
        auto rng = v | views::sliding(K);
        using Adapted = decltype(rng);
        test_size(rng, meta::bool_<sized_range<Base>>{});

        using IterTagOfBase = 
            meta::if_c<contiguous_iterator<iterator_t<Base>>,
                eastl::random_access_iterator_tag,
                iterator_tag_of<iterator_t<Base>>>;

        CPP_assert(same_as<
            IterTagOfBase,
            iterator_tag_of<iterator_t<Adapted>>>);

        auto it = ranges::begin(rng);
        test_common(rng, meta::bool_<common_range<Base>>{});

        for (auto i = 0; i <= N - K; ++i)
        {
            ::check_equal(*it++, views::iota(i, i + K));
        }
        CHECK(it == ranges::end(rng));

        test_prev(rng, it, meta::bool_<bidirectional_range<Base>>{});

        if (!broken_ebo)
        {
            CHECK(sizeof(it) == sizeof(size_compare<Base>));
        }
    }
}

void bug_975()
{
    eastl::vector<double> v{2.0, 2.0, 3.0, 1.0};
    eastl::vector<int> i{1, 2, 1, 2};
    eastl::vector<int> t{1, 1, 2, 2};
    {
        using namespace ranges;
        auto vals = views::zip(v, i, t);
        using T = eastl::tuple<double, int, int>;
        auto g = vals | views::chunk_by(
            [](T t1, T t2)
            {
                return eastl::get<2>(t1) == eastl::get<2>(t2);
            }
        );

        auto windows = views::sliding(g, 2);
        auto it = eastl::begin(windows);
        (void)it;
    }
}

int main()
{
    test_finite(to<eastl::slist<int>>());
    test_finite(to<eastl::list<int>>());
    test_finite(to<eastl::vector<int>>());
    test_finite(identity{});

    {
        // An infinite, cyclic range with cycle length == 1
        auto rng = views::repeat(5) | views::sliding(K);
        CPP_assert(random_access_range<decltype(rng)>);
        auto it = rng.begin();
        CPP_assert(random_access_iterator<decltype(it)>);
#if defined(__GNUC__) && !defined(__clang__) && \
    ((__GNUC__ == 6 && __GNUC_MINOR__ < 3) || __GNUC__ < 6)
        // Avoid https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78047
        {
            auto deref = *it;
            auto i = deref.begin();
            for (auto n = 0; n < K; ++n) {
                RANGES_ENSURE(i != deref.end());
                CHECK(*i == 5);
                ++i;
            }
            CHECK(i == deref.end());
        }
        auto it2 = next(it, 42);
        CHECK(it != it2);
        {
            auto deref = *it;
            auto i = deref.begin();
            for (auto n = 0; n < K; ++n) {
                RANGES_ENSURE(i != deref.end());
                CHECK(*i == 5);
                ++i;
            }
            CHECK(i == deref.end());
        }
#else
        ::check_equal(*it, views::repeat_n(5, K));
        auto it2 = next(it, 42);
        CHECK(it != it2);
        ::check_equal(*it2, views::repeat_n(5, K));
#endif
    }

    {
        // An infinite, cyclic range with cycle length == K
        auto rng = views::iota(0, K) | views::cycle | views::sliding(K);
        CPP_assert(random_access_range<decltype(rng)>);
        auto it = rng.begin();
        CPP_assert(random_access_iterator<decltype(it)>);
        for (auto i = 0; i < 42; ++i)
        {
            ::check_equal(*it++, {0,1,2});
            ::check_equal(*it++, {1,2,0});
            ::check_equal(*it++, {2,0,1});
        }
        auto it2 = next(it, 42 * K);
        ::check_equal(*it2, {0,1,2});
    }

    {
        // An infinite, cyclic range with cycle length > K
        auto rng = views::iota(0,7) | views::cycle | views::sliding(K);
        //[0,1,2],[1,2,3],[2,3,4],[3,4,5],[4,5,6],[5,6,0],[6,0,1],...
        auto it = rng.begin();
        CPP_assert(random_access_iterator<decltype(it)>);
        ::check_equal(*it, {0,1,2});
        ::check_equal(*next(it, 2), {2,3,4});
        ::check_equal(*next(it,16), {2,3,4});
        ::check_equal(*next(it,27), {6,0,1});
    }

    return ::test_result();
}
