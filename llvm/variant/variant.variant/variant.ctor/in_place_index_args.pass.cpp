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

// template <size_t I, class ...Args>
// constexpr explicit variant(in_place_index_t<I>, Args&&...);

#include "../../../test_convertible.h"
#include "rxx/variant.h"

#include <cassert>
#include <type_traits>

void test_ctor_sfinae() {
    {
        using V = __RXX variant<int>;
        static_assert(
            std::is_constructible<V, std::in_place_index_t<0>, int>::value, "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<0>, int>(), "");
    }
    {
        using V = __RXX variant<int, long, long long>;
        static_assert(
            std::is_constructible<V, std::in_place_index_t<1>, int>::value, "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<1>, int>(), "");
    }
    {
        using V = __RXX variant<int, long, int*>;
        static_assert(
            std::is_constructible<V, std::in_place_index_t<2>, int*>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<2>, int*>(), "");
    }
    { // args not convertible to type
        using V = __RXX variant<int, long, int*>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<0>, int*>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<0>, int*>(), "");
    }
    { // index not in variant
        using V = __RXX variant<int, long, int*>;
        static_assert(
            !std::is_constructible<V, std::in_place_index_t<3>, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_index_t<3>, int>(), "");
    }
}

void test_ctor_basic() {
    {
        constexpr __RXX variant<int> v(std::in_place_index<0>, 42);
        static_assert(v.index() == 0, "");
        static_assert(__RXX get<0>(v) == 42, "");
    }
    {
        constexpr __RXX variant<int, long, long> v(std::in_place_index<1>, 42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        constexpr __RXX variant<int, int const, long> v(
            std::in_place_index<1>, 42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_index<0>, x);
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == x);
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_index<1>, x);
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == x);
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_index<2>, x);
        assert(v.index() == 2);
        assert(__RXX get<2>(v) == x);
    }
}

int main(int, char**) {
    test_ctor_basic();
    test_ctor_sfinae();

    return 0;
}
