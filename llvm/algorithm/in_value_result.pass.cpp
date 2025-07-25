// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// template <class I, class T>
// struct in_value_result;

#include "../MoveOnly.h"
#include "rxx/algorithm/fold.h"

#include <cassert>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;

struct A {
    explicit A(int);
};
// no implicit conversion
static_assert(!std::is_constructible_v<xranges::in_value_result<A, A>,
              xranges::in_value_result<int, int>>);

struct B {
    B(int);
};
// implicit conversion
static_assert(std::is_constructible_v<xranges::in_value_result<B, B>,
    xranges::in_value_result<int, int>>);
static_assert(std::is_constructible_v<xranges::in_value_result<B, B>,
    xranges::in_value_result<int, int>&>);
static_assert(std::is_constructible_v<xranges::in_value_result<B, B>,
    xranges::in_value_result<int, int> const>);
static_assert(std::is_constructible_v<xranges::in_value_result<B, B>,
    xranges::in_value_result<int, int> const&>);

struct C {
    C(int&);
};
static_assert(!std::is_constructible_v<xranges::in_value_result<C, C>,
              xranges::in_value_result<int, int>&>);

// has to be convertible via const&
static_assert(std::is_convertible_v<xranges::in_value_result<int, int>&,
    xranges::in_value_result<long, long>>);
static_assert(std::is_convertible_v<xranges::in_value_result<int, int> const&,
    xranges::in_value_result<long, long>>);
static_assert(std::is_convertible_v<xranges::in_value_result<int, int>&&,
    xranges::in_value_result<long, long>>);
static_assert(std::is_convertible_v<xranges::in_value_result<int, int> const&&,
    xranges::in_value_result<long, long>>);

// should be move constructible
static_assert(
    std::is_move_constructible_v<xranges::in_value_result<MoveOnly, int>>);
static_assert(
    std::is_move_constructible_v<xranges::in_value_result<int, MoveOnly>>);

// should not be copy constructible with move-only type
static_assert(
    !std::is_copy_constructible_v<xranges::in_value_result<MoveOnly, int>>);
static_assert(
    !std::is_copy_constructible_v<xranges::in_value_result<int, MoveOnly>>);

struct NotConvertible {};
// conversions should not work if there is no conversion
static_assert(
    !std::is_convertible_v<xranges::in_value_result<NotConvertible, int>,
        xranges::in_value_result<int, int>>);
static_assert(
    !std::is_convertible_v<xranges::in_value_result<int, NotConvertible>,
        xranges::in_value_result<int, int>>);

template <class T>
struct ConvertibleFrom {
    constexpr ConvertibleFrom(T c) : content{c} {}
    T content;
};

constexpr bool test() {
    // Checks that conversion operations are correct.
    {
        xranges::in_value_result<int, double> res{10, 0.};
        assert(res.in == 10);
        assert(res.value == 0.);
        xranges::in_value_result<ConvertibleFrom<int>, ConvertibleFrom<double>>
            res2 = res;
        assert(res2.in.content == 10);
        assert(res2.value.content == 0.);
    }

    // Checks that conversions are possible when one of the types is move-only.
    {
        xranges::in_value_result<MoveOnly, int> res{MoveOnly{}, 2};
        assert(res.in.get() == 1);
        assert(res.value == 2);
        auto res2 = static_cast<xranges::in_value_result<MoveOnly, int>>(
            std::move(res));
        assert(res.in.get() == 0);
        assert(res2.in.get() == 1);
        assert(res2.value == 2);
    }

    // Checks that structured bindings get the correct values.
    {
        auto [in, value] = xranges::in_value_result<int, int>{1, 2};
        assert(in == 1);
        assert(value == 2);
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
