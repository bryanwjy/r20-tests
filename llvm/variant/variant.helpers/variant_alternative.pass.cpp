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

// template <size_t I, class T> struct variant_alternative; // undefined
// template <size_t I, class T> struct variant_alternative<I, const T>;
// template <size_t I, class T> struct variant_alternative<I, volatile T>;
// template <size_t I, class T> struct variant_alternative<I, const volatile T>;
// template <size_t I, class T>
//   using variant_alternative_t = typename variant_alternative<I, T>::type;
//
// template <size_t I, class... Types>
//    struct variant_alternative<I, variant<Types...>>;

#include "rxx/variant.h"

#include <type_traits>

template <class V, std::size_t I, class E>
void test() {
    static_assert(
        std::is_same_v<typename __RXX variant_alternative<I, V>::type, E>, "");
    static_assert(
        std::is_same_v<typename __RXX variant_alternative<I, V const>::type,
            E const>,
        "");
    static_assert(
        std::is_same_v<typename __RXX variant_alternative<I, V volatile>::type,
            E volatile>,
        "");
    static_assert(
        std::is_same_v<
            typename __RXX variant_alternative<I, V const volatile>::type,
            E const volatile>,
        "");
    static_assert(std::is_same_v<__RXX variant_alternative_t<I, V>, E>, "");
    static_assert(
        std::is_same_v<__RXX variant_alternative_t<I, V const>, E const>, "");
    static_assert(std::is_same_v<__RXX variant_alternative_t<I, V volatile>,
                      E volatile>,
        "");
    static_assert(
        std::is_same_v<__RXX variant_alternative_t<I, V const volatile>,
            E const volatile>,
        "");
}

template <typename T>
inline constexpr bool has_member_type = false;

template <typename T>
requires requires { typename T::type; }
inline constexpr bool has_member_type<T> = true;

int main(int, char**) {
    {
        using V = __RXX variant<int, void*, void const*, long double>;
        test<V, 0, int>();
        test<V, 1, void*>();
        test<V, 2, void const*>();
        test<V, 3, long double>();
        using V = __RXX variant<int, void*, void const*, long double>;
        static_assert(!has_member_type<__RXX variant_alternative<4, V>>);
    }

    return 0;
}
