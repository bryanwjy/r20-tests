// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//  A set of routines for testing the comparison operators of a type
//
//      FooOrder<expected-ordering>  All seven comparison operators, requires
//      C++20 or newer. FooComparison                All six pre-C++20
//      comparison operators FooEquality                  Equality operators
//      operator== and operator!=
//
//      AssertXAreNoexcept           static_asserts that the operations are all
//      noexcept. AssertXReturnBool            static_asserts that the
//      operations return bool. AssertOrderReturn            static_asserts that
//      the pre-C++20 comparison operations
//                                   return bool and operator<=> returns the
//                                   proper type.
//      AssertXConvertibleToBool     static_asserts that the operations return
//      something convertible to bool. testXValues                  returns the
//      result of the comparison of all operations.
//
//      AssertOrderConvertibleToBool doesn't exist yet. It will be implemented
//      when needed.

#ifndef TEST_COMPARISONS_H
#define TEST_COMPARISONS_H

#include "static_asserts.h"

#include <cassert>
#include <compare>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

// Test the consistency of the six basic comparison operators for values that
// are ordered or unordered.
template <class T, class U = T>
[[nodiscard]] constexpr bool testComparisonsComplete(
    T const& t1, U const& t2, bool isEqual, bool isLess, bool isGreater) {
    assert(((isEqual ? 1 : 0) + (isLess ? 1 : 0) + (isGreater ? 1 : 0) <= 1) &&
        "at most one of isEqual, isLess, and isGreater can be true");
    if (isEqual) {
        if (!(t1 == t2))
            return false;
        if (!(t2 == t1))
            return false;
        if ((t1 != t2))
            return false;
        if ((t2 != t1))
            return false;
        if ((t1 < t2))
            return false;
        if ((t2 < t1))
            return false;
        if (!(t1 <= t2))
            return false;
        if (!(t2 <= t1))
            return false;
        if ((t1 > t2))
            return false;
        if ((t2 > t1))
            return false;
        if (!(t1 >= t2))
            return false;
        if (!(t2 >= t1))
            return false;
    } else if (isLess) {
        if ((t1 == t2))
            return false;
        if ((t2 == t1))
            return false;
        if (!(t1 != t2))
            return false;
        if (!(t2 != t1))
            return false;
        if (!(t1 < t2))
            return false;
        if ((t2 < t1))
            return false;
        if (!(t1 <= t2))
            return false;
        if ((t2 <= t1))
            return false;
        if ((t1 > t2))
            return false;
        if (!(t2 > t1))
            return false;
        if ((t1 >= t2))
            return false;
        if (!(t2 >= t1))
            return false;
    } else if (isGreater) {
        if ((t1 == t2))
            return false;
        if ((t2 == t1))
            return false;
        if (!(t1 != t2))
            return false;
        if (!(t2 != t1))
            return false;
        if ((t1 < t2))
            return false;
        if (!(t2 < t1))
            return false;
        if ((t1 <= t2))
            return false;
        if (!(t2 <= t1))
            return false;
        if (!(t1 > t2))
            return false;
        if ((t2 > t1))
            return false;
        if (!(t1 >= t2))
            return false;
        if ((t2 >= t1))
            return false;
    } else { // unordered
        if ((t1 == t2))
            return false;
        if ((t2 == t1))
            return false;
        if (!(t1 != t2))
            return false;
        if (!(t2 != t1))
            return false;
        if ((t1 < t2))
            return false;
        if ((t2 < t1))
            return false;
        if ((t1 <= t2))
            return false;
        if ((t2 <= t1))
            return false;
        if ((t1 > t2))
            return false;
        if ((t2 > t1))
            return false;
        if ((t1 >= t2))
            return false;
        if ((t2 >= t1))
            return false;
    }

    return true;
}

// Test the six basic comparison operators for ordered values.
template <class T, class U = T>
[[nodiscard]] constexpr bool testComparisons(
    T const& t1, U const& t2, bool isEqual, bool isLess) {
    assert(!(isEqual && isLess) && "isEqual and isLess cannot be both true");
    bool isGreater = !isEqual && !isLess;
    return testComparisonsComplete(t1, t2, isEqual, isLess, isGreater);
}

//  Easy call when you can init from something already comparable.
template <class T, class Param>
[[nodiscard]] constexpr bool testComparisonsValues(Param val1, Param val2) {
    bool const isEqual = val1 == val2;
    bool const isLess = val1 < val2;
    bool const isGreater = val1 > val2;

    return testComparisonsComplete(
        T(val1), T(val2), isEqual, isLess, isGreater);
}

template <class T, class U = T>
constexpr void AssertComparisonsAreNoexcept() {
    ASSERT_NOEXCEPT(std::declval<T const&>() == std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() != std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() < std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() <= std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() > std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() >= std::declval<U const&>());
}

template <class T, class U = T>
constexpr void AssertComparisonsReturnBool() {
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() == std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() != std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() < std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() <= std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() > std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() >= std::declval<U const&>()), bool);
}

template <class T, class U = T>
void AssertComparisonsConvertibleToBool() {
    static_assert((std::is_convertible<decltype(std::declval<T const&>() ==
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() !=
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() <
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() <=
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() >
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() >=
                                           std::declval<U const&>()),
                      bool>::value),
        "");
}

template <class T, class U = T>
constexpr void AssertOrderAreNoexcept() {
    AssertComparisonsAreNoexcept<T, U>();
    ASSERT_NOEXCEPT(std::declval<T const&>() <=> std::declval<U const&>());
}

template <class Order, class T, class U = T>
constexpr void AssertOrderReturn() {
    AssertComparisonsReturnBool<T, U>();
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() <=> std::declval<U const&>()), Order);
}

template <class Order, class T, class U = T>
[[nodiscard]] constexpr bool testOrder(T const& t1, U const& t2, Order order) {
    bool equal = order == Order::equivalent;
    bool less = order == Order::less;
    bool greater = order == Order::greater;

    return (t1 <=> t2 == order) &&
        testComparisonsComplete(t1, t2, equal, less, greater);
}

template <class T, class Param>
[[nodiscard]] constexpr bool testOrderValues(Param val1, Param val2) {
    return testOrder(T(val1), T(val2), val1 <=> val2);
}

//  Test all two comparison operations for sanity
template <class T, class U = T>
[[nodiscard]] constexpr bool testEquality(
    T const& t1, U const& t2, bool isEqual) {
    if (isEqual) {
        if (!(t1 == t2))
            return false;
        if (!(t2 == t1))
            return false;
        if ((t1 != t2))
            return false;
        if ((t2 != t1))
            return false;
    } else /* not equal */
    {
        if ((t1 == t2))
            return false;
        if ((t2 == t1))
            return false;
        if (!(t1 != t2))
            return false;
        if (!(t2 != t1))
            return false;
    }

    return true;
}

//  Easy call when you can init from something already comparable.
template <class T, class Param>
[[nodiscard]] constexpr bool testEqualityValues(Param val1, Param val2) {
    bool const isEqual = val1 == val2;

    return testEquality(T(val1), T(val2), isEqual);
}

template <class T, class U = T>
void AssertEqualityAreNoexcept() {
    ASSERT_NOEXCEPT(std::declval<T const&>() == std::declval<U const&>());
    ASSERT_NOEXCEPT(std::declval<T const&>() != std::declval<U const&>());
}

template <class T, class U = T>
constexpr void AssertEqualityReturnBool() {
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() == std::declval<U const&>()), bool);
    ASSERT_SAME_TYPE(
        decltype(std::declval<T const&>() != std::declval<U const&>()), bool);
}

template <class T, class U = T>
void AssertEqualityConvertibleToBool() {
    static_assert((std::is_convertible<decltype(std::declval<T const&>() ==
                                           std::declval<U const&>()),
                      bool>::value),
        "");
    static_assert((std::is_convertible<decltype(std::declval<T const&>() !=
                                           std::declval<U const&>()),
                      bool>::value),
        "");
}

struct LessAndEqComp {
    int value;

    constexpr LessAndEqComp(int v) : value(v) {}

    friend constexpr bool operator<(
        LessAndEqComp const& lhs, LessAndEqComp const& rhs) {
        return lhs.value < rhs.value;
    }

    friend constexpr bool operator==(
        LessAndEqComp const& lhs, LessAndEqComp const& rhs) {
        return lhs.value == rhs.value;
    }
};

struct StrongOrder {
    int value;
    constexpr StrongOrder(int v) : value(v) {}
    friend std::strong_ordering operator<=>(StrongOrder, StrongOrder) = default;
};

struct WeakOrder {
    int value;
    constexpr WeakOrder(int v) : value(v) {}
    friend std::weak_ordering operator<=>(WeakOrder, WeakOrder) = default;
};

struct PartialOrder {
    int value;
    constexpr PartialOrder(int v) : value(v) {}
    friend constexpr std::partial_ordering operator<=>(
        PartialOrder lhs, PartialOrder rhs) {
        if (lhs.value == std::numeric_limits<int>::min() ||
            rhs.value == std::numeric_limits<int>::min())
            return std::partial_ordering::unordered;
        if (lhs.value == std::numeric_limits<int>::max() ||
            rhs.value == std::numeric_limits<int>::max())
            return std::partial_ordering::unordered;
        return lhs.value <=> rhs.value;
    }
    friend constexpr bool operator==(PartialOrder lhs, PartialOrder rhs) {
        return (lhs <=> rhs) == std::partial_ordering::equivalent;
    }
};

template <typename T1, typename T2 = T1>
concept HasOperatorEqual = requires(T1 t1, T2 t2) { t1 == t2; };

template <typename T1, typename T2 = T1>
concept HasOperatorGreaterThan = requires(T1 t1, T2 t2) { t1 > t2; };

template <typename T1, typename T2 = T1>
concept HasOperatorGreaterThanEqual = requires(T1 t1, T2 t2) { t1 >= t2; };
template <typename T1, typename T2 = T1>
concept HasOperatorLessThan = requires(T1 t1, T2 t2) { t1 < t2; };

template <typename T1, typename T2 = T1>
concept HasOperatorLessThanEqual = requires(T1 t1, T2 t2) { t1 <= t2; };

template <typename T1, typename T2 = T1>
concept HasOperatorNotEqual = requires(T1 t1, T2 t2) { t1 != t2; };

template <typename T1, typename T2 = T1>
concept HasOperatorSpaceship = requires(T1 t1, T2 t2) { t1 <=> t2; };

struct NonComparable {};
static_assert(!std::equality_comparable<NonComparable>);
static_assert(!HasOperatorEqual<NonComparable>);
static_assert(!HasOperatorGreaterThan<NonComparable>);
static_assert(!HasOperatorGreaterThanEqual<NonComparable>);
static_assert(!HasOperatorLessThan<NonComparable>);
static_assert(!HasOperatorLessThanEqual<NonComparable>);
static_assert(!HasOperatorNotEqual<NonComparable>);
static_assert(!HasOperatorSpaceship<NonComparable>);

class EqualityComparable {
public:
    constexpr EqualityComparable(int value) : value_{value} {};

    friend constexpr bool operator==(EqualityComparable const&,
        EqualityComparable const&) noexcept = default;

private:
    int value_;
};
static_assert(std::equality_comparable<EqualityComparable>);
static_assert(HasOperatorEqual<EqualityComparable>);
static_assert(HasOperatorNotEqual<EqualityComparable>);

class ThreeWayComparable {
public:
    constexpr ThreeWayComparable(int value) : value_{value} {};

    friend constexpr bool operator==(ThreeWayComparable const&,
        ThreeWayComparable const&) noexcept = default;
    friend constexpr std::strong_ordering operator<=>(ThreeWayComparable const&,
        ThreeWayComparable const&) noexcept = default;

private:
    int value_;
};
static_assert(std::equality_comparable<ThreeWayComparable>);
static_assert(std::three_way_comparable<ThreeWayComparable>);
static_assert(HasOperatorEqual<ThreeWayComparable>);
static_assert(HasOperatorGreaterThan<ThreeWayComparable>);
static_assert(HasOperatorGreaterThanEqual<ThreeWayComparable>);
static_assert(HasOperatorLessThan<ThreeWayComparable>);
static_assert(HasOperatorLessThanEqual<ThreeWayComparable>);
static_assert(HasOperatorNotEqual<ThreeWayComparable>);
static_assert(HasOperatorSpaceship<ThreeWayComparable>);

#endif // TEST_COMPARISONS_H
