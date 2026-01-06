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

// template <class T, class U> constexpr bool operator>=(const optional<T>& x,
// const U& v); template <class T, class U> constexpr bool operator>=(const U&
// v, const optional<T>& x);

#include "../../test_comparisons.h"
#include "rxx/optional.h"

// Test SFINAE.
static_assert(
    HasOperatorGreaterThanEqual<__RXX optional<ThreeWayComparable>, int>);
static_assert(HasOperatorGreaterThanEqual<__RXX optional<ThreeWayComparable>,
    ThreeWayComparable>);

static_assert(!HasOperatorGreaterThanEqual<__RXX optional<NonComparable>,
    NonComparable>);
static_assert(!HasOperatorGreaterThanEqual<__RXX optional<ThreeWayComparable>,
    NonComparable>);
static_assert(!HasOperatorGreaterThanEqual<__RXX optional<NonComparable>,
    ThreeWayComparable>);

static_assert(
    HasOperatorGreaterThanEqual<int, __RXX optional<ThreeWayComparable>>);
static_assert(HasOperatorGreaterThanEqual<ThreeWayComparable,
    __RXX optional<ThreeWayComparable>>);

static_assert(!HasOperatorGreaterThanEqual<NonComparable,
    __RXX optional<NonComparable>>);
static_assert(!HasOperatorGreaterThanEqual<NonComparable,
    __RXX optional<ThreeWayComparable>>);
static_assert(!HasOperatorGreaterThanEqual<ThreeWayComparable,
    __RXX optional<NonComparable>>);

using __RXX optional;

struct X {
    int i_;

    constexpr X(int i) : i_(i) {}
};

constexpr bool operator>=(X const& lhs, X const& rhs) {
    return lhs.i_ >= rhs.i_;
}

int main(int, char**) {
    {
        typedef X T;
        typedef optional<T> O;

        constexpr T val(2);
        constexpr O o1;      // disengaged
        constexpr O o2{1};   // engaged
        constexpr O o3{val}; // engaged

        static_assert(!(o1 >= T(1)), "");
        static_assert((o2 >= T(1)), ""); // equal
        static_assert((o3 >= T(1)), "");
        static_assert(!(o2 >= val), "");
        static_assert((o3 >= val), ""); // equal
        static_assert(!(o3 >= T(3)), "");

        static_assert((T(1) >= o1), "");
        static_assert((T(1) >= o2), ""); // equal
        static_assert(!(T(1) >= o3), "");
        static_assert((val >= o2), "");
        static_assert((val >= o3), ""); // equal
        static_assert((T(3) >= o3), "");
    }
    {
        using O = optional<int>;
        constexpr O o1(42);
        static_assert(o1 >= 42l, "");
        static_assert(!(11l >= o1), "");
    }
    {
        using O = optional<int const>;
        constexpr O o1(42);
        static_assert(o1 >= 42, "");
        static_assert(!(11 >= o1), "");
    }

    return 0;
}
