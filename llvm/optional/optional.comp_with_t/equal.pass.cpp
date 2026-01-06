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

// template <class T, class U> constexpr bool operator==(const optional<T>& x,
// const U& v); template <class T, class U> constexpr bool operator==(const U&
// v, const optional<T>& x);

#include "../../test_comparisons.h"
#include "rxx/optional.h"

static_assert(HasOperatorEqual<int, __RXX optional<int>>);
static_assert(HasOperatorEqual<int, __RXX optional<EqualityComparable>>);
static_assert(
    HasOperatorEqual<EqualityComparable, __RXX optional<EqualityComparable>>);

static_assert(!HasOperatorEqual<NonComparable, __RXX optional<NonComparable>>);
static_assert(
    !HasOperatorEqual<NonComparable, __RXX optional<EqualityComparable>>);

static_assert(HasOperatorEqual<__RXX optional<int>, int>);
static_assert(HasOperatorEqual<__RXX optional<EqualityComparable>, int>);
static_assert(
    HasOperatorEqual<__RXX optional<EqualityComparable>, EqualityComparable>);

static_assert(
    !HasOperatorEqual<__RXX optional<NonComparable>, NonComparable>);
static_assert(
    !HasOperatorEqual<__RXX optional<EqualityComparable>, NonComparable>);

using __RXX optional;

struct X {
    int i_;

    constexpr X(int i) : i_(i) {}
};

constexpr bool operator==(X const& lhs, X const& rhs) {
    return lhs.i_ == rhs.i_;
}

int main(int, char**) {
    {
        typedef X T;
        typedef optional<T> O;

        constexpr T val(2);
        constexpr O o1;      // disengaged
        constexpr O o2{1};   // engaged
        constexpr O o3{val}; // engaged

        static_assert(!(o1 == T(1)), "");
        static_assert((o2 == T(1)), "");
        static_assert(!(o3 == T(1)), "");
        static_assert((o3 == T(2)), "");
        static_assert((o3 == val), "");

        static_assert(!(T(1) == o1), "");
        static_assert((T(1) == o2), "");
        static_assert(!(T(1) == o3), "");
        static_assert((T(2) == o3), "");
        static_assert((val == o3), "");
    }
    {
        using O = optional<int>;
        constexpr O o1(42);
        static_assert(o1 == 42l, "");
        static_assert(!(101l == o1), "");
    }
    {
        using O = optional<int const>;
        constexpr O o1(42);
        static_assert(o1 == 42, "");
        static_assert(!(101 == o1), "");
    }

    return 0;
}
