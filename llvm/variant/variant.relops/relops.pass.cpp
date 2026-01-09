// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types>
// constexpr bool
// operator==(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator!=(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator<(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator>(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator<=(variant<Types...> const&, variant<Types...> const&) noexcept;
//
// template <class ...Types>
// constexpr bool
// operator>=(variant<Types...> const&, variant<Types...> const&) noexcept;

#include "../../test_comparisons.h"
#include "rxx/variant.h"

#include <cassert>
#include <type_traits>
#include <utility>

#if 1

// C++26 features?
// Test SFINAE.

// ==
static_assert(HasOperatorEqual<__RXX variant<EqualityComparable>>);
static_assert(HasOperatorEqual<__RXX variant<EqualityComparable, int, long>>);

static_assert(!HasOperatorEqual<__RXX variant<NonComparable>>);
static_assert(
    !HasOperatorEqual<__RXX variant<NonComparable, EqualityComparable>>);

// >
static_assert(HasOperatorGreaterThan<__RXX variant<ThreeWayComparable>>);
static_assert(
    HasOperatorGreaterThan<__RXX variant<ThreeWayComparable, int, long>>);

static_assert(!HasOperatorGreaterThan<__RXX variant<NonComparable>>);
static_assert(!HasOperatorGreaterThan<
    __RXX variant<NonComparable, ThreeWayComparable>>);

// >=
static_assert(HasOperatorGreaterThanEqual<__RXX variant<ThreeWayComparable>>);
static_assert(HasOperatorGreaterThanEqual<
    __RXX variant<ThreeWayComparable, int, long>>);

static_assert(!HasOperatorGreaterThanEqual<__RXX variant<NonComparable>>);
static_assert(!HasOperatorGreaterThanEqual<
    __RXX variant<NonComparable, ThreeWayComparable>>);

// <
static_assert(HasOperatorLessThan<__RXX variant<ThreeWayComparable>>);
static_assert(
    HasOperatorLessThan<__RXX variant<ThreeWayComparable, int, long>>);

static_assert(!HasOperatorLessThan<__RXX variant<NonComparable>>);
static_assert(
    !HasOperatorLessThan<__RXX variant<NonComparable, ThreeWayComparable>>);

// <=
static_assert(HasOperatorLessThanEqual<__RXX variant<ThreeWayComparable>>);
static_assert(
    HasOperatorLessThanEqual<__RXX variant<ThreeWayComparable, int, long>>);

static_assert(!HasOperatorLessThanEqual<__RXX variant<NonComparable>>);
static_assert(!HasOperatorLessThanEqual<
    __RXX variant<NonComparable, ThreeWayComparable>>);

// !=
static_assert(HasOperatorNotEqual<__RXX variant<EqualityComparable>>);
static_assert(
    HasOperatorNotEqual<__RXX variant<EqualityComparable, int, long>>);

static_assert(!HasOperatorNotEqual<__RXX variant<NonComparable>>);
static_assert(
    !HasOperatorNotEqual<__RXX variant<NonComparable, EqualityComparable>>);

#endif

#if RXX_WITH_EXCEPTIONS
struct MakeEmptyT {
    MakeEmptyT() = default;
    MakeEmptyT(MakeEmptyT&&) { throw 42; }
    MakeEmptyT& operator=(MakeEmptyT&&) { throw 42; }
};
inline bool operator==(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}
inline bool operator!=(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}
inline bool operator<(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}
inline bool operator<=(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}
inline bool operator>(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}
inline bool operator>=(MakeEmptyT const&, MakeEmptyT const&) {
    assert(false);
    return false;
}

template <class Variant>
void makeEmpty(Variant& v) {
    Variant v2(std::in_place_type<MakeEmptyT>);
    try {
        v = std::move(v2);
        assert(false);
    } catch (...) {
        assert(v.valueless_by_exception());
    }
}
#endif // TEST_HAS_NO_EXCEPTIONS

struct MyBool {
    bool value;
    constexpr explicit MyBool(bool v) : value(v) {}
    constexpr operator bool() const noexcept { return value; }
};

struct ComparesToMyBool {
    int value = 0;
};
inline constexpr MyBool operator==(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value == RHS.value);
}
inline constexpr MyBool operator!=(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value != RHS.value);
}
inline constexpr MyBool operator<(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value < RHS.value);
}
inline constexpr MyBool operator<=(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value <= RHS.value);
}
inline constexpr MyBool operator>(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value > RHS.value);
}
inline constexpr MyBool operator>=(
    ComparesToMyBool const& LHS, ComparesToMyBool const& RHS) noexcept {
    return MyBool(LHS.value >= RHS.value);
}

template <class T1, class T2>
void test_equality_basic() {
    {
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{42});
        constexpr V v2(std::in_place_index<0>, T1{42});
        static_assert(v1 == v2, "");
        static_assert(v2 == v1, "");
        static_assert(!(v1 != v2), "");
        static_assert(!(v2 != v1), "");
    }
    {
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{42});
        constexpr V v2(std::in_place_index<0>, T1{43});
        static_assert(!(v1 == v2), "");
        static_assert(!(v2 == v1), "");
        static_assert(v1 != v2, "");
        static_assert(v2 != v1, "");
    }
    {
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{42});
        constexpr V v2(std::in_place_index<1>, T2{42});
        static_assert(!(v1 == v2), "");
        static_assert(!(v2 == v1), "");
        static_assert(v1 != v2, "");
        static_assert(v2 != v1, "");
    }
    {
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<1>, T2{42});
        constexpr V v2(std::in_place_index<1>, T2{42});
        static_assert(v1 == v2, "");
        static_assert(v2 == v1, "");
        static_assert(!(v1 != v2), "");
        static_assert(!(v2 != v1), "");
    }
}

void test_equality() {
    test_equality_basic<int, long>();
    test_equality_basic<ComparesToMyBool, int>();
    test_equality_basic<int, ComparesToMyBool>();
    test_equality_basic<ComparesToMyBool, ComparesToMyBool>();
#if RXX_WITH_EXCEPTIONS
    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        V v2;
        makeEmpty(v2);
        assert(!(v1 == v2));
        assert(!(v2 == v1));
        assert(v1 != v2);
        assert(v2 != v1);
    }
    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        makeEmpty(v1);
        V v2;
        assert(!(v1 == v2));
        assert(!(v2 == v1));
        assert(v1 != v2);
        assert(v2 != v1);
    }
    {
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        makeEmpty(v1);
        V v2;
        makeEmpty(v2);
        assert(v1 == v2);
        assert(v2 == v1);
        assert(!(v1 != v2));
        assert(!(v2 != v1));
    }
#endif
}

template <class Var>
constexpr bool test_less(
    Var const& l, Var const& r, bool expect_less, bool expect_greater) {
    static_assert(std::is_same_v<decltype(l < r), bool>, "");
    static_assert(std::is_same_v<decltype(l <= r), bool>, "");
    static_assert(std::is_same_v<decltype(l > r), bool>, "");
    static_assert(std::is_same_v<decltype(l >= r), bool>, "");

    return ((l < r) == expect_less) && (!(l >= r) == expect_less) &&
        ((l > r) == expect_greater) && (!(l <= r) == expect_greater);
}

template <class T1, class T2>
void test_relational_basic() {
    { // same index, same value
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{1});
        constexpr V v2(std::in_place_index<0>, T1{1});
        static_assert(test_less(v1, v2, false, false), "");
    }
    { // same index, value < other_value
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{0});
        constexpr V v2(std::in_place_index<0>, T1{1});
        static_assert(test_less(v1, v2, true, false), "");
    }
    { // same index, value > other_value
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{1});
        constexpr V v2(std::in_place_index<0>, T1{0});
        static_assert(test_less(v1, v2, false, true), "");
    }
    { // LHS.index() < RHS.index()
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<0>, T1{0});
        constexpr V v2(std::in_place_index<1>, T2{0});
        static_assert(test_less(v1, v2, true, false), "");
    }
    { // LHS.index() > RHS.index()
        using V = __RXX variant<T1, T2>;
        constexpr V v1(std::in_place_index<1>, T2{0});
        constexpr V v2(std::in_place_index<0>, T1{0});
        static_assert(test_less(v1, v2, false, true), "");
    }
}

void test_relational() {
    test_relational_basic<int, long>();
    test_relational_basic<ComparesToMyBool, int>();
    test_relational_basic<int, ComparesToMyBool>();
    test_relational_basic<ComparesToMyBool, ComparesToMyBool>();
#if RXX_WITH_EXCEPTIONS
    { // LHS.index() < RHS.index(), RHS is empty
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        V v2;
        makeEmpty(v2);
        assert(test_less(v1, v2, false, true));
    }
    { // LHS.index() > RHS.index(), LHS is empty
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        makeEmpty(v1);
        V v2;
        assert(test_less(v1, v2, true, false));
    }
    { // LHS.index() == RHS.index(), LHS and RHS are empty
        using V = __RXX variant<int, MakeEmptyT>;
        V v1;
        makeEmpty(v1);
        V v2;
        makeEmpty(v2);
        assert(test_less(v1, v2, false, false));
    }
#endif
}

int main(int, char**) {
    test_equality();
    test_relational();

    return 0;
}
