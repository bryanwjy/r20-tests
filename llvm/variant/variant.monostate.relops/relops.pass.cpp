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

// constexpr bool operator<(monostate, monostate) noexcept { return false; }
// constexpr bool operator>(monostate, monostate) noexcept { return false; }
// constexpr bool operator<=(monostate, monostate) noexcept { return true; }
// constexpr bool operator>=(monostate, monostate) noexcept { return true; }
// constexpr bool operator==(monostate, monostate) noexcept { return true; }
// constexpr bool operator!=(monostate, monostate) noexcept { return false; }
// constexpr strong_ordering operator<=>(monostate, monostate) noexcept { return
// strong_ordering::equal; } // since C++20

#include "../../test_comparisons.h"
#include "rxx/variant.h"

#include <cassert>

constexpr bool test() {
    using M = __RXX monostate;
    constexpr M m1{};
    constexpr M m2{};
    assert(testComparisons(m1, m2, /*isEqual*/ true, /*isLess*/ false));
    AssertComparisonsAreNoexcept<M>();

    assert(testOrder(m1, m2, std::strong_ordering::equal));
    AssertOrderAreNoexcept<M>();

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
