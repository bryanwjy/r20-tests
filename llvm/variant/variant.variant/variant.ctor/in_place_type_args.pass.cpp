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

// template <class Tp, class ...Args>
// constexpr explicit variant(in_place_type_t<Tp>, Args&&...);

#include "../../../test_convertible.h"
#include "rxx/variant.h"

#include <cassert>
#include <type_traits>

void test_ctor_sfinae() {
    {
        using V = __RXX variant<int>;
        static_assert(
            std::is_constructible<V, std::in_place_type_t<int>, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<int>, int>(), "");
    }
    {
        using V = __RXX variant<int, long, long long>;
        static_assert(
            std::is_constructible<V, std::in_place_type_t<long>, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<long>, int>(), "");
    }
    {
        using V = __RXX variant<int, long, int*>;
        static_assert(
            std::is_constructible<V, std::in_place_type_t<int*>, int*>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<int*>, int*>(), "");
    }
    { // duplicate type
        using V = __RXX variant<int, long, int>;
        static_assert(
            !std::is_constructible<V, std::in_place_type_t<int>, int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<int>, int>(), "");
    }
    { // args not convertible to type
        using V = __RXX variant<int, long, int*>;
        static_assert(
            !std::is_constructible<V, std::in_place_type_t<int>, int*>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<int>, int*>(), "");
    }
    { // type not in variant
        using V = __RXX variant<int, long, int*>;
        static_assert(!std::is_constructible<V, std::in_place_type_t<long long>,
                          int>::value,
            "");
        static_assert(
            !test_convertible<V, std::in_place_type_t<long long>, int>(), "");
    }
}

void test_ctor_basic() {
    {
        constexpr __RXX variant<int> v(std::in_place_type<int>, 42);
        static_assert(v.index() == 0, "");
        static_assert(__RXX get<0>(v) == 42, "");
    }
    {
        constexpr __RXX variant<int, long> v(std::in_place_type<long>, 42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        constexpr __RXX variant<int, int const, long> v(
            std::in_place_type<int const>, 42);
        static_assert(v.index() == 1, "");
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_type<int const>, x);
        assert(v.index() == 0);
        assert(__RXX get<0>(v) == x);
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_type<int volatile>, x);
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == x);
    }
    {
        using V = __RXX variant<int const, int volatile, int>;
        int x = 42;
        V v(std::in_place_type<int>, x);
        assert(v.index() == 2);
        assert(__RXX get<2>(v) == x);
    }
}

int main(int, char**) {
    test_ctor_basic();
    test_ctor_sfinae();

    return 0;
}
