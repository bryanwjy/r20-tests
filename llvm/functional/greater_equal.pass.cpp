// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <functional>

// ranges::greater_equal

#include "rxx/functional/greater_equal.h"

#include "../MoveOnly.h"
#include "../compare_types.h"
#include "../pointer_comparison_test_helper.h"

#include <cassert>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;

struct NotTotallyOrdered {
    friend bool operator<(NotTotallyOrdered const&, NotTotallyOrdered const&);
};

static_assert(!std::is_invocable_v<xranges::greater_equal, NotTotallyOrdered,
              NotTotallyOrdered>);
// The following becomes true in C++23 thanks to P2404R3
static_assert(std::is_invocable_v<xranges::greater_equal, int, MoveOnly>);
static_assert(std::is_invocable_v<xranges::greater_equal, explicit_operators,
    explicit_operators>);

static_assert(requires { typename xranges::greater_equal::is_transparent; });

constexpr bool test() {
    auto fn = xranges::greater_equal();

    assert(fn(MoveOnly(42), MoveOnly(42)));

    ForwardingTestObject a;
    ForwardingTestObject b;
    assert(fn(a, b));
    assert(!fn(std::move(a), std::move(b)));

    assert(fn(2, 2));
    assert(fn(2, 1));
    assert(!fn(1, 2));

    assert(fn(2, 1L));

    return true;
}

int main(int, char**) {

    test();
    static_assert(test());

    // test total ordering of int* for greater_equal<int*> and
    // greater_equal<void>.
    do_pointer_comparison_test(xranges::greater_equal());

    return 0;
}
