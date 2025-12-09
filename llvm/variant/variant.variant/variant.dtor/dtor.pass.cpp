// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// ~variant();

#include "rxx/variant.h"

#include <cassert>
#include <type_traits>

struct NonTDtor {
    int* count;
    constexpr NonTDtor(int* a, int*) : count(a) {}
    constexpr ~NonTDtor() { ++*count; }
};
static_assert(!std::is_trivially_destructible<NonTDtor>::value, "");

struct NonTDtor1 {
    int* count;
    constexpr NonTDtor1(int*, int* b) : count(b) {}
    constexpr ~NonTDtor1() { ++*count; }
};
static_assert(!std::is_trivially_destructible<NonTDtor1>::value, "");

struct TDtor {
    constexpr TDtor() = default;
    constexpr TDtor(TDtor const&) {} // non-trivial copy
    constexpr ~TDtor() = default;
};
static_assert(!std::is_trivially_copy_constructible<TDtor>::value, "");
static_assert(std::is_trivially_destructible<TDtor>::value, "");

constexpr bool test() {
    {
        using V = __RXX variant<int, long, TDtor>;
        static_assert(std::is_trivially_destructible<V>::value, "");
        [[maybe_unused]] V v(std::in_place_index<2>);
    }
    {
        using V = __RXX variant<NonTDtor, int, NonTDtor1>;
        static_assert(!std::is_trivially_destructible<V>::value, "");
        {
            int count0 = 0;
            int count1 = 0;
            {
                V v(std::in_place_index<0>, &count0, &count1);
                assert(count0 == 0);
                assert(count1 == 0);
            }
            assert(count0 == 1);
            assert(count1 == 0);
        }
        {
            int count0 = 0;
            int count1 = 0;
            { V v(std::in_place_index<1>); }
            assert(count0 == 0);
            assert(count1 == 0);
        }
        {
            int count0 = 0;
            int count1 = 0;
            {
                V v(std::in_place_index<2>, &count0, &count1);
                assert(count0 == 0);
                assert(count1 == 0);
            }
            assert(count0 == 0);
            assert(count1 == 1);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
