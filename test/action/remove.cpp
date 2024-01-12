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
#include <EASTL/string.h>
#include <EARanges/action/remove.hpp>

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

struct Data
{
    int i;

    Data() = default;
    explicit Data(int j) : i(j) {}

    bool operator==(const Data& other) const {
        return other.i == i;
    }
    bool operator!=(const Data& other) const {
        return other.i != i;
    }
};

void simple_test()
{
    //TODO: Come fa a compilare anche con std nonostante nessuno lo includa, da usare compile build score.
    std::vector<Data> list;
    list.emplace_back(Data{1});
    list.emplace_back(Data{2});
    list.emplace_back(Data{3});
    list.emplace_back(Data{4});

    Data d2{2};

    const auto remove_data = actions::remove(d2);

    list |= remove_data;
    check_equal(list, {Data{1}, Data{3}, Data{4}});

    list |= actions::remove(3, &Data::i);
    check_equal(list, {Data{1}, Data{4}});
}

void string_test()
{
    eastl::vector<eastl::string> list = {"aaa", "bbb", "ccc"};
    list |= actions::remove("bbb");
    check_equal(list, {"aaa", "ccc"});
}

int main()
{
    simple_test();
    string_test();

    return ::test_result();
}
