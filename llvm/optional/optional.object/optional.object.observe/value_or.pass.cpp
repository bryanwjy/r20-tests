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

// template <class U> constexpr T optional<T>::value_or(U&& v) &&;

#include "rxx/optional.h"

#include <cassert>

using __RXX optional;
using std::in_place;

struct Y {
    int i_;

    constexpr Y(int i) : i_(i) {}
};

struct X {
    int i_;

    constexpr X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(x.i_) { x.i_ = 0; }
    constexpr X(Y const& y) : i_(y.i_) {}
    constexpr X(Y&& y) : i_(y.i_ + 1) {}
    friend constexpr bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_;
    }
};

struct Z {
    int i_, j_;
    constexpr Z(int i, int j) : i_(i), j_(j) {}
    friend constexpr bool operator==(Z const& z1, Z const& z2) {
        return z1.i_ == z2.i_ && z1.j_ == z2.j_;
    }
};

constexpr int test() {
    {
        optional<X> opt(in_place, 2);
        Y y(3);
        assert(std::move(opt).value_or(y) == 2);
        assert(*opt == 0);
    }
    {
        optional<X> opt(in_place, 2);
        assert(std::move(opt).value_or(Y(3)) == 2);
        assert(*opt == 0);
    }
    {
        optional<X> opt;
        Y y(3);
        assert(std::move(opt).value_or(y) == 3);
        assert(!opt);
    }
    {
        optional<X> opt;
        assert(std::move(opt).value_or(Y(3)) == 4);
        assert(!opt);
    }
    {
        optional<X> opt;
        assert(std::move(opt).value_or({Y(3)}) == 4);
        assert(!opt);
    }
    {
        optional<Z> opt;
        assert((std::move(opt).value_or({2, 3}) == Z{2, 3}));
        assert(!opt);
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        int y = 2;
        optional<int&> opt;
        assert(std::move(opt).value_or(y) == 2);
        assert(!opt);
    }
#endif
    return 0;
}

int main(int, char**) {
    static_assert(test() == 0);

    return 0;
}
