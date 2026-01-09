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
//   optional(const optional<U>& rhs);

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

template <class T, class U>
constexpr void test(optional<U> const& rhs, bool is_going_to_throw = false) {
    bool rhs_engaged = static_cast<bool>(rhs);
#if RXX_WITH_EXCEPTIONS
    try {
        optional<T> lhs = rhs;
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
        if (rhs_engaged)
            assert(*lhs == *rhs);
    } catch (int i) {
        assert(i == 6);
    }
#else
    if (is_going_to_throw)
        return;
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
#endif
}

class X {
    int i_;

public:
    constexpr X(int i) : i_(i) {}
    constexpr X(X const& x) : i_(x.i_) {}
    constexpr ~X() { i_ = 0; }
    friend constexpr bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_;
    }
};

class Y {
    int i_;

public:
    constexpr Y(int i) : i_(i) {}

    friend constexpr bool operator==(Y const& x, Y const& y) {
        return x.i_ == y.i_;
    }
};

int count = 0;

class Z {
    int i_;

public:
    Z(int i) : i_(i) { RXX_THROW(6); }

    friend bool operator==(Z const& x, Z const& y) { return x.i_ == y.i_; }
};

template <class T, class U>
constexpr bool test_all() {
    {
        optional<U> rhs;
        test<T>(rhs);
    }
    {
        optional<U> rhs(U{3});
        test<T>(rhs);
    }
    return true;
}

int main(int, char**) {
    test_all<int, short>();
    test_all<X, int>();
    test_all<Y, int>();
    static_assert(test_all<int, short>());
    static_assert(test_all<X, int>());
    static_assert(test_all<Y, int>());
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef Z T;
        typedef int U;
        optional<U> rhs(U{3});
        test<T>(rhs, true);
    }

    static_assert(
        !(std::is_constructible<optional<X>, optional<Y> const&>::value), "");

    return 0;
}
