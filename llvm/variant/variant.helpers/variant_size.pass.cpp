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

// template <class T> struct variant_size; // undefined
// template <class T> struct variant_size<const T>;
// template <class T> struct variant_size<volatile T>;
// template <class T> struct variant_size<const volatile T>;
// template <class T> constexpr size_t variant_size_v
//     = variant_size<T>::value;

#include "rxx/variant.h"

template <class V, std::size_t E>
void test() {
    static_assert(__RXX variant_size<V>::value == E, "");
    static_assert(__RXX variant_size<V const>::value == E, "");
    static_assert(__RXX variant_size<V volatile>::value == E, "");
    static_assert(__RXX variant_size<V const volatile>::value == E, "");
    static_assert(__RXX variant_size_v<V> == E, "");
    static_assert(__RXX variant_size_v<V const> == E, "");
    static_assert(__RXX variant_size_v<V volatile> == E, "");
    static_assert(__RXX variant_size_v<V const volatile> == E, "");
    static_assert(std::is_base_of<std::integral_constant<std::size_t, E>,
                      __RXX variant_size<V>>::value,
        "");
};

int main(int, char**) {
    test<__RXX variant<>, 0>();
    test<__RXX variant<void*>, 1>();
    test<__RXX variant<long, long, void*, double>, 4>();

    return 0;
}
