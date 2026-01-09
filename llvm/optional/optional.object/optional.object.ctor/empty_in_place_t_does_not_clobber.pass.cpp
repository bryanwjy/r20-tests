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

// constexpr optional(in_place_t);

// Test that the SFINAE "is_constructible<value_type>" isn't evaluated by the
// in_place_t constructor with no arguments when the Clang is trying to check
// copy constructor.

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

struct Wrapped {
    struct Inner {
        bool Dummy = true;
    };
    __RXX optional<Inner> inner;
};

int main(int, char**) {
    static_assert(std::is_default_constructible<Wrapped::Inner>::value, "");
    Wrapped::Inner i;
    Wrapped w;
    w.inner.emplace();
    assert(w.inner.has_value());

    return 0;
}
