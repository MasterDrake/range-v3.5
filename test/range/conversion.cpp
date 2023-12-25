// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/vector.h>
#include <EASTL/numeric_limits.h>

#include <EASTL/ranges/action/sort.hpp>
#include <EASTL/ranges/core.hpp>
#include <EASTL/ranges/range/conversion.hpp>
#include <EASTL/ranges/view/indices.hpp>
#include <EASTL/ranges/view/iota.hpp>
#include <EASTL/ranges/view/take.hpp>
#include <EASTL/ranges/view/transform.hpp>
#include <EASTL/ranges/view/zip.hpp>
#include <EASTL/ranges/view/reverse.hpp>

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

template<typename T>
struct vector_like
{
private:
    eastl::vector<T> data_;
public:
    using size_type = std::size_t;
   //TODO:23) Eastl needs a eastl::allocator implemention compatible for std::allocator. Luckily we didnd't need it here :D
   //using allocator_type = eastl::allocator<T>;

    vector_like() = default;
    template<typename I>
    vector_like(I first, I last)
      : data_(first, last)
    {}
    template<typename I>
    void assign(I first, I last)
    {
        data_.assign(first, last);
    }

    auto begin()
    {
        return data_.begin();
    }
    auto end()
    {
        return data_.end();
    }
    auto begin() const
    {
        return data_.begin();
    }
    auto end() const
    {
        return data_.end();
    }
    size_type size() const
    {
        return data_.size();
    }
    size_type capacity() const
    {
        return data_.capacity();
    }
    size_type max_size() const
    {
       // return data_.max_size();
        //TODO:23) Since eastl::vector doesn't have a max_size() member function, I just used the cpp reference one:https://en.cppreference.com/w/cpp/container/vector/max_size
        //It's not perfect or correct but at least it works so be wary!

        /*This value typically reflects the theoretical limit on the size of the container, at most std::numeric_limits<difference_type>::max(). 
        At runtime, the size of the container may be limited to a value smaller than max_size() by the amount of RAM available.*/
        return eastl::numeric_limits<size_type>::max();
    }
    auto& operator[](size_type n)
    {
        return data_[n];
    }
    auto& operator[](size_type n) const
    {
        return data_[n];
    }

    size_type last_reservation{};
    size_type reservation_count{};

    void reserve(size_type n)
    {
        data_.reserve(n);
        last_reservation = n;
        ++reservation_count;
    }
};

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
template<typename I>
vector_like(I, I) -> vector_like<ranges::iter_value_t<I>>;

template<typename Rng, typename CI = ranges::range_common_iterator_t<Rng>, typename = decltype(eastl::map{CI{}, CI{}})>
void test_zip_to_map(Rng && rng, int)
{
    using namespace ranges;
#ifdef RANGES_WORKAROUND_MSVC_779708
    auto m = static_cast<Rng &&>(rng) | to<eastl::map>();
#else  // ^^^ workaround / no workaround vvv
    auto m = static_cast<Rng &&>(rng) | to<eastl::map>;
#endif // RANGES_WORKAROUND_MSVC_779708
    CPP_assert(same_as<decltype(m), eastl::map<int, int>>);
}
#endif
template<typename Rng>
void test_zip_to_map(Rng &&, long)
{}

template<typename K, typename V>
struct map_like : eastl::map<K, V>
{
    template<typename Iter>
    map_like(Iter f, Iter l) : eastl::map<K, V>(f, l)
    {}
};

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
template<typename Iter>
map_like(Iter, Iter) -> map_like<typename ranges::iter_value_t<Iter>::first_type, typename ranges::iter_value_t<Iter>::second_type>;
#endif

int main()
{
    using namespace ranges;

    {
        auto lst0 = views::ints | views::transform([](int i) { return i * i; }) | views::take(10) | to<eastl::list>();
        CPP_assert(same_as<decltype(lst0), eastl::list<int>>);
        ::check_equal(lst0, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81});
    }

#ifndef RANGES_WORKAROUND_MSVC_779708 // "workaround" is a misnomer; there's no
                                      // workaround.
    {
        auto lst1 = views::ints | views::transform([](int i) { return i * i; }) | views::take(10) | to<eastl::list>;
        CPP_assert(same_as<decltype(lst1), eastl::list<int>>);
        ::check_equal(lst1, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81});
    }
#endif // RANGES_WORKAROUND_MSVC_779708

    {
        auto vec0 = views::ints | views::transform([](int i) { return i * i; }) | views::take(10) | to_vector | actions::sort(eastl::greater<int>{});
        CPP_assert(same_as<decltype(vec0), eastl::vector<int>>);
        ::check_equal(vec0, {81, 64, 49, 36, 25, 16, 9, 4, 1, 0});
    }

    {
        auto vec1 = views::ints | views::transform([](int i) { return i * i; }) |
                    views::take(10) | to<eastl::vector<long>>() |
                    actions::sort(eastl::greater<long>{});
        CPP_assert(same_as<decltype(vec1), eastl::vector<long>>);
        ::check_equal(vec1, {81, 64, 49, 36, 25, 16, 9, 4, 1, 0});
    }

#ifndef RANGES_WORKAROUND_MSVC_779708
    {
        auto vec2 = views::ints | views::transform([](int i) { return i * i; }) |
                    views::take(10) | to<eastl::vector<long>> |
                    actions::sort(eastl::greater<long>{});
        CPP_assert(same_as<decltype(vec2), eastl::vector<long>>);
        ::check_equal(vec2, {81, 64, 49, 36, 25, 16, 9, 4, 1, 0});
    }
#endif // RANGES_WORKAROUND_MSVC_779708

    {
        const std::size_t N = 4096;
        auto vl = views::iota(0, int{N}) | to<vector_like<int>>();
        CPP_assert(same_as<decltype(vl), vector_like<int>>);
        CHECK(vl.reservation_count == std::size_t{1});
        CHECK(vl.last_reservation == N);
    }

    // https://github.com/ericniebler/range-v3/issues/1145
    {
        auto r1 = views::indices(std::uintmax_t{100});
        auto r2 = views::zip(r1, r1);

#ifdef RANGES_WORKAROUND_MSVC_779708
        auto m = r2 | ranges::to<eastl::map<std::uintmax_t, std::uintmax_t>>();
#else // ^^^ workaround / no workaround vvv
        auto m = r2 | ranges::to<eastl::map<std::uintmax_t, std::uintmax_t>>;
#endif // RANGES_WORKAROUND_MSVC_779708
        CPP_assert(same_as<decltype(m), eastl::map<std::uintmax_t, std::uintmax_t>>);
    }

    // Transform a range-of-ranges into a container of containers
    {
        auto r = views::ints(1, 4) | views::transform([](int i) { return views::ints(i, i + 3); });

        auto m = r | ranges::to<eastl::vector<eastl::vector<int>>>();
        CPP_assert(same_as<decltype(m), eastl::vector<eastl::vector<int>>>);
        CHECK(m.size() == 3u);
        check_equal(m[0], {1, 2, 3});
        check_equal(m[1], {2, 3, 4});
        check_equal(m[2], {3, 4, 5});
    }

    // Use ranges::to in a closure with an action
    {
#ifdef RANGES_WORKAROUND_MSVC_779708
        auto closure = ranges::to<eastl::vector>() | actions::sort;
#else // ^^^ workaround / no workaround vvv
        auto closure = ranges::to<eastl::vector> | actions::sort;
#endif // RANGES_WORKAROUND_MSVC_779708

        auto r = views::ints(1, 4) | views::reverse;
        auto m = r | closure;

        CPP_assert(same_as<decltype(m), eastl::vector<int>>);
        CHECK(m.size() == 3u);
        check_equal(m, {1, 2, 3});
    }

    test_zip_to_map(views::zip(views::ints, views::iota(0, 10)), 0);

    // https://github.com/ericniebler/range-v3/issues/1544
    {
        eastl::vector<eastl::vector<int>> d;
        auto m = views::transform(d, views::all);
        auto v = ranges::to<eastl::vector<eastl::vector<int>>>(m);
        check_equal(d, v);
    }

    {
        eastl::vector<eastl::pair<int, int>> v = {{1, 2}, {3, 4}};
        auto m1 = ranges::to<map_like<int, int>>(v);
        auto m2 = v | ranges::to<map_like<int, int>>();

        CPP_assert(same_as<decltype(m1), map_like<int, int>>);
        CPP_assert(same_as<decltype(m2), map_like<int, int>>);
        check_equal(m1, eastl::map<int, int>{{1, 2}, {3, 4}});
        check_equal(m1, m2);

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        auto m3 = ranges::to<map_like>(v);
        auto m4 = v | ranges::to<map_like>();
        CPP_assert(same_as<decltype(m3), map_like<int, int>>);
        CPP_assert(same_as<decltype(m4), map_like<int, int>>);
        check_equal(m1, m3);
        check_equal(m1, m4);
#endif
    }

    return ::test_result();
}
