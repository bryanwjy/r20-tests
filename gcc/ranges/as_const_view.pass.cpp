// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "rxx/ranges/as_const_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/chunk_view.h"
#include "rxx/ranges/empty_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/join_view.h"
#include "rxx/ranges/ref_view.h"
#include "rxx/ranges/reverse_view.h"
#include "rxx/ranges/transform_view.h"

#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto v2 = x | xviews::filter([](int x) { return (x % 2) == 0; });
    auto c = xranges::cbegin(v2);
    auto v = x | xviews::filter([](int x) { return (x % 2) == 0; }) |
        xviews::as_const;

    using ty = decltype(v);
    static_assert(xranges::constant_range<ty>);
    static_assert(!xranges::constant_range<decltype(v.base())>);
    static_assert(std::same_as<xranges::range_reference_t<ty>, int const&>);
    static_assert(
        std::same_as<xranges::range_reference_t<decltype(v.base())>, int&>);

    assert(xranges::equal(v, (int[]){2, 4, 6, 8, 10}));
    assert(xranges::equal(v | xviews::reverse, (int[]){10, 8, 6, 4, 2}));

    return true;
}

constexpr bool test02() {
    int x[] = {1, 2, 3};
    std::same_as<xranges::empty_view<int const>> auto v1 =
        xviews::empty<int> | xviews::as_const;
    std::same_as<xranges::ref_view<int const[3]>> auto v2 =
        x | xviews::as_const;
    std::same_as<xranges::ref_view<int const[3]>> auto v3 =
        std::as_const(x) | xviews::as_const;
    std::same_as<xranges::ref_view<int const[3]>> auto v4 =
        std::as_const(x) | xviews::all | xviews::as_const;
    std::same_as<std::span<int const>> auto v5 =
        std::span{x, x + 3} | xviews::as_const;
    std::same_as<xranges::as_const_view<
        xranges::chunk_view<xranges::ref_view<int[3]>>>> auto v6 =
        x | xviews::chunk(2) | xviews::as_const;
    assert(v6.size() == 2);

    return true;
}

void test03() {
    // PR libstdc++/109525
    std::vector<int> v;
    std::same_as<xranges::ref_view<std::vector<int> const>> auto r =
        xviews::as_const(v);

    // PR libstdc++/119135
    std::same_as<xranges::ref_view<std::vector<int> const>> auto r2 =
        xviews::as_const(xviews::all(v));
}

void test04() {
    // PR libstdc++/115046 - meta-recursion with join_view and as_const_view
    int x[3] = {1, 2, 3};
    auto v = x | xviews::chunk(3) | xviews::transform(xviews::as_const) |
        xviews::join;
    assert(xranges::equal(v, x));
}

int main() {
    static_assert(test01());
    static_assert(test02());
    test03();
    test04();
}
