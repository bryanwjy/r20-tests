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

// template <class U, class... Args>
//     constexpr
//     explicit optional(in_place_t, initializer_list<U> il, Args&&... args);

#include "rxx/optional.h"

#include <cassert>
#include <memory>
#include <type_traits>
#include <vector>

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
    constexpr Y(std::initializer_list<int> il)
        : i_(il.begin()[0])
        , j_(il.begin()[1]) {}

    friend constexpr bool operator==(Y const& x, Y const& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

class Z {
    int i_;
    int j_ = 0;

public:
    Z() : i_(0) {}
    Z(int i) : i_(i) {}
    Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {
        RXX_THROW(6);
    }

    friend bool operator==(Z const& x, Z const& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

int main(int, char**) {
    {
        static_assert(
            !std::is_constructible<X, std::initializer_list<int>&>::value, "");
        static_assert(!std::is_constructible<optional<X>,
                          std::initializer_list<int>&>::value,
            "");
    }
    {
        optional<std::vector<int>> opt(in_place, {3, 1});
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        optional<std::vector<int>> opt(in_place, {3, 1}, std::allocator<int>());
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        static_assert(std::is_constructible<optional<Y>,
                          std::initializer_list<int>&>::value,
            "");
        constexpr optional<Y> opt(in_place, {3, 1});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y{3, 1}, "");

        struct test_constexpr_ctor : public optional<Y> {
            constexpr test_constexpr_ctor(
                in_place_t, std::initializer_list<int> i)
                : optional<Y>(in_place, i) {}
        };
    }
#if RXX_WITH_EXCEPTIONS
    {
        static_assert(std::is_constructible<optional<Z>,
                          std::initializer_list<int>&>::value,
            "");
        try {
            optional<Z> opt(in_place, {3, 1});
            assert(false);
        } catch (int i) {
            assert(i == 6);
        }
    }
#endif

    return 0;
}
