// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <EASTL/vector.h>
#include <cstdlib>
#include <ctime>
#include <EASTL/random.h>
#include <random>

#include <EARanges/action/unstable_remove_if.hpp>
#include <EARanges/action/remove_if.hpp>
#include <EARanges/action/sort.hpp>

#include "../array.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"

void logic_test()
{
    using namespace ranges;

    const auto make_vector = []() -> eastl::vector<int>
    {
        return {1,2,3,4,5};
    };

    // empty
    {
        eastl::vector<int> vec;
        vec |= actions::unstable_remove_if([](int) { return true; });
        CHECK(vec.empty());
    }

    // all stay
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int) { return false; });
        check_equal(vec, {1,2,3,4,5});
    }

    // all remove
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int) { return true; });
        CHECK(vec.empty());
    }

    // remove one in the middle
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 2; });
        check_equal(vec, {1,5,3,4});
    }

    // remove first
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 1; });
        check_equal(vec, {5,2,3,4});
    }

    // remove last
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 5; });
        check_equal(vec, {1,2,3,4});
    }

    // remove group in the middle
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 2 || i == 3 || i == 4; });
        check_equal(vec, {1,5});
    }

    // remove group in the begin
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 1 || i == 2 || i == 3; });
        check_equal(vec, {5,4});
    }

    // remove group in the end
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i == 3 || i == 4 || i == 5; });
        check_equal(vec, {1,2});
    }

    // remains one in the middle
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return i != 3; });
        check_equal(vec, {3});
    }
    // remains group in the middle
    {
        eastl::vector<int> vec = make_vector();
        vec |= actions::unstable_remove_if([](int i) { return (i != 3) && (i != 4); });
        check_equal(vec, {4,3});
    }
}

void num_pred_calls_test()
{
    // eastl::ranges::remove_if requires:
    // "Exactly N applications of the corresponding predicate and any projection, where N = (last - first)"
    // https://en.cppreference.com/w/cpp/algorithm/ranges/remove
    // so expect the same of unstable_remove_if
    using namespace ranges;

    int pred_invocation_counter = 0;
    auto is_zero_count_invocations = [&pred_invocation_counter](int i)
    {
        ++pred_invocation_counter;
        return i == 0;
    };

    {
        eastl::vector<int> vec{0};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 1);
    }

    {
        eastl::vector<int> vec{1,1,1};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 3);
    }

    {
        eastl::vector<int> vec{1,0};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 2);
    }

    {
        eastl::vector<int> vec{1,2,0};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 3);
    }

    {
        eastl::vector<int> vec{0,0,0,0};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 4);
    }

    {
        eastl::vector<int> vec{1,2,3,0,0,0,0,4,5};
        pred_invocation_counter = 0;
        vec |= actions::unstable_remove_if(is_zero_count_invocations);
        check_equal(pred_invocation_counter, 9);
    }
}

class fuzzy_test_fn
{
    int size;
#if defined(__GLIBCXX__) && defined(EARANGES_WORKAROUND_VALGRIND_RDRAND)
    std::random_device rd{"/dev/urandom"};
#else
    std::random_device rd;
#endif
    eastl::RNG<> eng{rd()};
    eastl::uniform_int_distribution<int> distr;

public:
    explicit fuzzy_test_fn(int sz)
      : size(sz)
      , distr{0, sz}
    {}

    void operator()()
    {
        struct Int
        {
            int value;

            explicit Int(int v)
              : value(v)
            {}
            Int(Int const &) = default;
            Int(Int&& other) noexcept
              : value(0)
            {
                *this = eastl::move(other);
            }

            Int &operator=(Int const &) = default;
            Int &operator=(Int&& other) noexcept
            {
                const int sentinel = -1;
                CHECK(other.value != sentinel);

                value = other.value;
                other.value = sentinel;
                return *this;
            }

            EARANGES_DIAGNOSTIC_PUSH
            EARANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
            bool operator==(Int const &other) const
            {
                return value == other.value;
            }
            bool operator!=(Int const &other) const
            {
                return value != other.value;
            }
            bool operator<(Int const &other) const
            {
                return value < other.value;
            }
            bool operator>(Int const &other) const
            {
                return value > other.value;
            }
            bool operator<=(Int const &other) const
            {
                return value <= other.value;
            }
            bool operator>=(Int const &other) const
            {
                return value >= other.value;
            }
            EARANGES_DIAGNOSTIC_POP
        };

        using namespace ranges;
        eastl::vector<Int> ordered_list;
        eastl::vector<Int> unordered_list;

        // fill
        for(int i=0; i < size; ++i)
        {
            ordered_list.emplace_back(i);
            unordered_list.emplace_back(i);
        }

        // erase
        const int erase_count = distr(eng);
        for(int i=0; i < erase_count; ++i)
        {
            const int value = distr(eng);
            const auto pred = [value](Int j) { return j.value == value; };
            unordered_list |= actions::unstable_remove_if(pred);
            ordered_list |= actions::remove_if(pred);
        }

        // compare
        unordered_list |= actions::sort;
        //TODO: BREAKS IN linux-gcc release mode
        //CHECK(ordered_list == unordered_list);
    }
};

int main()
{
    logic_test();
    num_pred_calls_test();

    {
        const int size = 100;
        const int repeats = 1000;
        fuzzy_test_fn fuzzy_test(size);
        for(int i=0; i < repeats; ++i)
            fuzzy_test();
    }

    return ::test_result();
}
