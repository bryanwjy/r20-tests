// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Implementations of well-known functions in mathematics that are useful for
// testing algorithms.

#ifndef LIBCXX_TEST_MATHS_H
#define LIBCXX_TEST_MATHS_H

#include "rxx/algorithm/minmax.h"
#include "rxx/ranges/concepts.h"
#include "rxx/ranges/empty_view.h"
#include "rxx/ranges/primitives.h"

#include <cassert>
#include <concepts>
#include <utility>
#include <vector>

namespace xranges = rxx::ranges;

template <xranges::forward_range R>
constexpr xranges::range_value_t<R> triangular_sum(R& input) {
    assert(not xranges::empty(input));
    auto [min, max] = xranges::minmax_element(input);
    return static_cast<xranges::range_value_t<R>>(
        (static_cast<double>(xranges::distance(input)) / 2) * (*min + *max));
}

template <std::integral I>
constexpr I factorial(I const n) {
    assert(n >= 0);
    auto result = I(1);
    for (auto i = I(1); i <= n; ++i) {
        result *= i;
    }

    return result;
}
static_assert(factorial(0) == 1);
static_assert(factorial(1) == 1);
static_assert(factorial(2) == 2);
static_assert(factorial(3) == 6);
static_assert(factorial(4) == 24);
static_assert(factorial(5) == 120);

template <std::integral I>
constexpr I fibonacci(I const n) {
    assert(n >= 0);

    auto result = I(0);
    auto prev = I(1);
    for (auto i = I(0); i < n; ++i) {
        result += std::exchange(prev, result);
    }
    return result;
}
static_assert(fibonacci(0) == 0);
static_assert(fibonacci(1) == 1);
static_assert(fibonacci(2) == 1);
static_assert(fibonacci(3) == 2);
static_assert(fibonacci(4) == 3);
static_assert(fibonacci(5) == 5);
static_assert(fibonacci(6) == 8);
static_assert(fibonacci(7) == 13);
static_assert(fibonacci(8) == 21);
static_assert(fibonacci(9) == 34);

#endif // LIBCXX_TEST_MATHS_H
