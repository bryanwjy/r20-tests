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

// template <size_t I, class Up, class ...Args>
// constexpr explicit
// variant(in_place_index_t<I>, initializer_list<Up>, Args&&...);

#include "../../../test_convertible.h"
#include "rxx/variant.h"

#include <cassert>
#include <string>
#include <type_traits>

struct InitList {
    std::size_t size;
    constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
    std::size_t size;
    int value;
    constexpr InitListArg(std::initializer_list<int> il, int v)
        : size(il.size())
        , value(v) {}
};

void test_ctor_sfinae() {
    using IL = std::initializer_list<int>;
    { // just init list
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            std::is_constructible<V, std::in_place_index_t<0>, IL>::value, "");
        static_assert(!test_convertible<V, std::in_place_index_t<0>, IL>(), "");
    }
    { // too many arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<0>, IL, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<0>, IL, int>(), "");
    }
    { // too few arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<1>, IL>::value, "");
        static_assert(!test_convertible<V, std::in_place_index_t<1>, IL>(), "");
    }
    { // init list and arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            std::is_constructible<V, std::in_place_index_t<1>, IL, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<1>, IL, int>(), "");
    }
    { // not constructible from arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<2>, IL>::value, "");
        static_assert(!test_convertible<V, std::in_place_index_t<2>, IL>(), "");
    }
    { // index not in variant
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<3>, IL>::value, "");
        static_assert(!test_convertible<V, std::in_place_index_t<3>, IL>(), "");
    }
}

void test_ctor_basic() {
    {
        constexpr __RXX variant<InitList, InitListArg, InitList> v(
            std::in_place_index<0>, {1, 2, 3});
        static_assert(v.index() == 0, "");
        static_assert(__RXX get<0>(v).size == 3, "");
    }
    {
        constexpr __RXX variant<InitList, InitListArg, InitList> v(
            std::in_place_index<2>, {1, 2, 3});
        static_assert(v.index() == 2, "");
        static_assert(__RXX get<2>(v).size == 3, "");
    }
    {
        constexpr __RXX variant<InitList, InitListArg, InitListArg> v(
            std::in_place_index<1>, {1, 2, 3, 4}, 42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v).size == 4, "");
        static_assert(__RXX get<1>(v).value == 42, "");
    }
}

int main(int, char**) {
    test_ctor_basic();
    test_ctor_sfinae();

    return 0;
}
