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

#include "rxx/ranges/slide_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/join_view.h"

#include <array>
#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    auto v1 = std::array{1, 2} | xviews::slide(1);
    auto const i0 = v1.begin(), i1 = v1.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < v1.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(v1.end() - i1 == 1);
    assert(i1 - v1.end() == -1);
    assert(xranges::equal(std::move(v1) | xviews::join, (int[]){1, 2}));

    int x[] = {1, 2, 3, 4};
    auto v2 = x | xviews::slide(2);
    auto i2 = v2.begin();
    i2 += 2;
    i2 -= -1;
    assert(i2 == v2.end());
    assert(xranges::size(v2) == 3);
    assert(xranges::size(std::as_const(v2)) == 3);
    assert(xranges::equal(v2,
        (std::initializer_list<int>[]){
            {1, 2},
            {2, 3},
            {3, 4}
    },
        xranges::equal));

    int y[] = {1, 2, 3, 4, 5};
    auto const v3 = y | xviews::slide(3);
    assert(xranges::size(v3) == 3);
    for (unsigned i = 0; i < xranges::size(x); i++) {
        assert(&v3[i][0] == &y[i] + 0);
        assert(&v3[i][1] == &y[i] + 1);
        assert(&v3[i][2] == &y[i] + 2);
    }

    // LWG 3848 - slide_view etc missing base accessor
    static_assert(sizeof(decltype(v3.base())) > 0);

    auto const v5 = y | xviews::slide(5);
    assert(xranges::size(v5) == 1);
    assert(xranges::equal(v5 | xviews::join, y));

    auto const v6 = y | xviews::slide(6);
    assert(xranges::empty(v6));

    return true;
}

constexpr bool test02() {
    using rxx::tests::test_forward_range;
    using rxx::tests::test_input_range;
    using rxx::tests::test_random_access_range;

    using ty1 = xranges::slide_view<xviews::all_t<test_forward_range<int>>>;
    static_assert(xranges::forward_range<ty1>);
    static_assert(!xranges::bidirectional_range<ty1>);
    static_assert(!xranges::sized_range<ty1>);

    using ty2 =
        xranges::slide_view<xviews::all_t<test_random_access_range<int>>>;
    static_assert(xranges::random_access_range<ty2>);
    static_assert(xranges::sized_range<ty2>);

    return true;
}

constexpr bool test03() {
    auto v = xviews::iota(0, 4) | xviews::filter([](auto) { return true; }) |
        xviews::slide(2);
    using ty = decltype(v);
    static_assert(xranges::forward_range<ty>);
    static_assert(xranges::common_range<ty>);
    static_assert(!xranges::sized_range<ty>);
    assert(v.begin() == v.begin());
    assert(v.begin() != v.end());
    assert(xranges::next(v.begin(), 3) == v.end());
    auto it = v.begin();
    ++it;
    it++;
    assert(xranges::next(it) == v.end());
    it--;
    --it;
    assert(it == v.begin());

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
}
