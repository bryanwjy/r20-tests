// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// constexpr outer-iterator(outer-iterator<!Const> i)
//   requires Const && convertible_to<iterator_t<View>, iterator_t<Base>>

#include "../types.h"
#include "rxx/ranges.h"

#include <string_view>
#include <type_traits>
#include <utility>

// outer-iterator<Const = true>

template <class Iter>
concept IsConstOuterIter = requires(Iter i) {
    { *(*i).begin() } -> std::same_as<char const&>;
};
static_assert(IsConstOuterIter<OuterIterConst>);

static_assert(std::convertible_to<xranges::iterator_t<SplitViewDiff>,
    xranges::iterator_t<SplitViewDiff const>>);

// outer-iterator<Const = false>

template <class Iter>
concept IsNonConstOuterIter = requires(Iter i) {
    { *(*i).begin() } -> std::same_as<char&>;
};
static_assert(IsNonConstOuterIter<OuterIterNonConst>);

static_assert(!std::is_constructible_v<OuterIterNonConst, OuterIterConst>);

constexpr bool test() {
    [[maybe_unused]] OuterIterConst i(OuterIterNonConst{});

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
