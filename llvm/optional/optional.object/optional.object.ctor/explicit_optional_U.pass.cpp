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
//   explicit optional(optional<U>&& rhs);

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

template <class T, class U>
constexpr void test(optional<U>&& rhs, bool is_going_to_throw = false) {
    static_assert(
        !(std::is_convertible<optional<U>&&, optional<T>>::value), "");
    bool rhs_engaged = static_cast<bool>(rhs);
#if RXX_WITH_EXCEPTIONS
    try {
        optional<T> lhs(std::move(rhs));
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
    } catch (int i) {
        assert(i == 6);
    }
#else
    if (is_going_to_throw)
        return;
    optional<T> lhs(std::move(rhs));
    assert(static_cast<bool>(lhs) == rhs_engaged);
#endif
}

class X {
    int i_;

public:
    constexpr explicit X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(x.i_) { x.i_ = 0; }
    constexpr ~X() { i_ = 0; }
    friend constexpr bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_;
    }
};

int count = 0;

class Z {
public:
    explicit Z(int) { RXX_THROW(6); }
};

constexpr bool test() {
    {
        optional<int> rhs;
        test<X>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<X>(std::move(rhs));
    }

    return true;
}

int main(int, char**) {
    static_assert(test());
    test();
    {
        optional<int> rhs;
        test<Z>(std::move(rhs));
    }
    {
        optional<int> rhs(3);
        test<Z>(std::move(rhs), true);
    }

    return 0;
}
