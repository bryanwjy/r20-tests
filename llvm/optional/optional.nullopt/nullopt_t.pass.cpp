// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14
// <optional>

// struct nullopt_t{see below};
// inline constexpr nullopt_t nullopt(unspecified);

// [optional.nullopt]/2:
//   Type nullopt_t shall not have a default constructor or an initializer-list
//   constructor, and shall not be an aggregate.

#include "rxx/optional.h"

#include <type_traits>

using __RXX nullopt;
using __RXX nullopt_t;

constexpr bool test() {
    nullopt_t foo{nullopt};
    (void)foo;
    return true;
}

int main(int, char**) {
    static_assert(std::is_empty_v<nullopt_t>);
    static_assert(!std::is_default_constructible_v<nullopt_t>);

    static_assert(std::is_same_v<nullopt_t const, decltype(nullopt)>);
    static_assert(test());

    return 0;
}
