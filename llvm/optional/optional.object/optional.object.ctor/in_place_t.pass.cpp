// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: c++03, c++11, c++14

// <optional>

// template <class... Args>
//   constexpr explicit optional(in_place_t, Args&&... args);

#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;
using std::in_place;
using std::in_place_t;

class X {
    int i_;
    int j_ = 0;

public:
    X() : i_(0) {}
    X(int i) : i_(i) {}
    X(int i, int j) : i_(i), j_(j) {}

    ~X() {}

    friend bool operator==(X const& x, X const& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

class Y {
    int i_;
    int j_ = 0;

public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(int i, int j) : i_(i), j_(j) {}

    friend constexpr bool operator==(Y const& x, Y const& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

class Z {
public:
    Z(int) { RXX_THROW(6); }
};

int main(int, char**) {
    {
        constexpr optional<int> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor : public optional<int> {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : optional<int>(in_place, i) {}
        };
    }
    {
        optional<int const> opt(in_place, 5);
        assert(*opt == 5);
    }
    {
        optional<X> const opt(in_place);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X());
    }
    {
        optional<X> const opt(in_place, 5);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5));
    }
    {
        optional<X> const opt(in_place, 5, 4);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5, 4));
    }
    {
        constexpr optional<Y> opt(in_place);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(), "");

        struct test_constexpr_ctor : public optional<Y> {
            constexpr test_constexpr_ctor(in_place_t) : optional<Y>(in_place) {}
        };
    }
    {
        constexpr optional<Y> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5), "");

        struct test_constexpr_ctor : public optional<Y> {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : optional<Y>(in_place, i) {}
        };
    }
    {
        constexpr optional<Y> opt(in_place, 5, 4);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5, 4), "");

        struct test_constexpr_ctor : public optional<Y> {
            constexpr test_constexpr_ctor(in_place_t, int i, int j)
                : optional<Y>(in_place, i, j) {}
        };
    }
#if RXX_WITH_EXCEPTIONS
    {
        try {
            optional<Z> const opt(in_place, 1);
            assert(false);
        } catch (int i) {
            assert(i == 6);
        }
    }
#endif

    return 0;
}
