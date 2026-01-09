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

// template <class U>
//   optional(optional<U>&& rhs);

#include "rxx/optional.h"

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

using __RXX optional;

template <class T, class U>
constexpr void test(optional<U>&& rhs, bool is_going_to_throw = false) {
    bool rhs_engaged = static_cast<bool>(rhs);
#if RXX_WITH_EXCEPTIONS
    try {
        optional<T> lhs = std::move(rhs);
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
    } catch (int i) {
        assert(i == 6);
    }
#else
    if (is_going_to_throw)
        return;
    optional<T> lhs = std::move(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
#endif
}

class X {
    int i_;

public:
    constexpr X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(std::exchange(x.i_, 0)) {}
    constexpr ~X() { i_ = 0; }
    friend constexpr bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_;
    }
};

struct Z {
    Z(int) { RXX_THROW(6); }
};

template <class T, class U>
constexpr bool test_all() {
    {
        optional<T> rhs;
        test<U>(std::move(rhs));
    }
    {
        optional<T> rhs(short{3});
        test<U>(std::move(rhs));
    }
    return true;
}

int main(int, char**) {
    test_all<short, int>();
    test_all<int, X>();
    static_assert(test_all<short, int>());
    static_assert(test_all<int, X>());
    {
        optional<int> rhs;
        test<Z>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<Z>(std::move(rhs), true);
    }

    static_assert(
        !(std::is_constructible<optional<X>, optional<Z>>::value), "");

    return 0;
}
