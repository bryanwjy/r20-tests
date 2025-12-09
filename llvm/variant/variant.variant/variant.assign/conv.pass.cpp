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

// template <class T>
// variant& operator=(T&&) noexcept(see below);

#include "rxx/variant.h"

#include <memory>
#include <string>

int main(int, char**) {
    static_assert(
        !std::is_assignable<__RXX variant<int, int>, int>::value, "");
    static_assert(
        !std::is_assignable<__RXX variant<long, long long>, int>::value, "");
    static_assert(!std::is_assignable<__RXX variant<char>, int>::value, "");

    static_assert(
        !std::is_assignable<__RXX variant<std::string, float>, int>::value,
        "");
    static_assert(
        !std::is_assignable<__RXX variant<std::string, double>, int>::value,
        "");
    static_assert(
        !std::is_assignable<__RXX variant<std::string, bool>, int>::value,
        "");

    static_assert(!std::is_assignable<__RXX variant<int, bool>,
                      decltype("meow")>::value,
        "");
    static_assert(!std::is_assignable<__RXX variant<int, bool const>,
                      decltype("meow")>::value,
        "");

    static_assert(
        std::is_assignable<__RXX variant<bool>, std::true_type>::value, "");
    static_assert(!std::is_assignable<__RXX variant<bool>,
                      std::unique_ptr<char>>::value,
        "");
    static_assert(
        !std::is_assignable<__RXX variant<bool>, decltype(nullptr)>::value,
        "");

    return 0;
}
