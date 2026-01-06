// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++26

// <optional>

// template <class T> class optional<T&>::iterator;
// template <class T> class optional<T&>::const_iterator;
// template <class T>
// constexpr bool ranges::enable_borrowed_range<optional<T&>> = true;

#include "rxx/optional.h"
#include "rxx/ranges.h"

#include <cassert>

#if RXX_SUPPORTS_OPTIONAL_REFERENCE
template <typename T>
void borrowed_range() {
    static_assert(std::ranges::enable_borrowed_range<__RXX optional<T&>>);
    static_assert(__RXX ranges::range<__RXX optional<T&>> ==
        __RXX ranges::borrowed_range<__RXX optional<T&>>);
}

void test_borrowed_range() {
    borrowed_range<int>();
    borrowed_range<int const>();
    borrowed_range<int[]>();
    borrowed_range<int[10]>();
    borrowed_range<int()>();
}

#endif
