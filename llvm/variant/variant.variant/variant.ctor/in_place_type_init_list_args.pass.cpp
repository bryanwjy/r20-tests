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

// template <class Tp, class Up, class ...Args>
// constexpr explicit
// variant(in_place_type_t<Tp>, initializer_list<Up>, Args&&...);

#include "../../../test_convertible.h"
#include "rxx/variant.h"

#include <cassert>
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
            std::is_constructible<V, std::in_place_type_t<InitList>, IL>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<InitList>, IL>(), "");
    }
    { // too many arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(!std::is_constructible<V, std::in_place_type_t<InitList>,
                          IL, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<InitList>, IL, int>(),
            "");
    }
    { // too few arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(!std::is_constructible<V,
                          std::in_place_type_t<InitListArg>, IL>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<InitListArg>, IL>(), "");
    }
    { // init list and arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(std::is_constructible<V,
                          std::in_place_type_t<InitListArg>, IL, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<InitListArg>, IL, int>(),
            "");
    }
    { // not constructible from arguments
        using V = __RXX variant<InitList, InitListArg, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_type_t<int>, IL>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<int>, IL>(), "");
    }
    { // duplicate types in variant
        using V = __RXX variant<InitListArg, InitListArg, int>;
        static_assert(!std::is_constructible<V,
                          std::in_place_type_t<InitListArg>, IL, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<InitListArg>, IL, int>(),
            "");
    }
}

void test_ctor_basic() {
    {
        constexpr __RXX variant<InitList, InitListArg> v(
            std::in_place_type<InitList>, {1, 2, 3});
        static_assert(v.index() == 0, "");
        static_assert(__RXX get<0>(v).size == 3, "");
    }
    {
        constexpr __RXX variant<InitList, InitListArg> v(
            std::in_place_type<InitListArg>, {1, 2, 3, 4}, 42);
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
