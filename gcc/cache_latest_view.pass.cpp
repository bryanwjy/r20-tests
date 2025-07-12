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

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/cache_latest_view.h"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace ranges = std::ranges;
namespace views = std::views;

constexpr bool test01() {
    int xs[] = {1, 2, 3, 4, 5};
    auto v = xs | rxx::views::cache_latest;
    assert(ranges::equal(v, xs));
    assert(ranges::size(v) == 5);

    auto it = v.begin();
    auto st = v.end();
    assert(st - it == 5);
    assert(it - st == -5);
    it++;
    assert(st - it == 4);
    assert(it - st == -4);

    auto jt = v.begin();
    ranges::iter_swap(it, jt);
    assert(ranges::equal(xs, (int[]){2, 1, 3, 4, 5}));
    int n = ranges::iter_move(it);
    assert(n == 1);
    ranges::iter_swap(it, jt);

    auto w = views::iota(1, 6) | rxx::views::cache_latest;
    assert(ranges::equal(w, xs));

    return true;
}

constexpr bool test02() {
    // Motivating example from P3138R5
    int xs[] = {1, 2, 3, 4, 5};
    int transform_count = 0;
    auto v = xs | views::transform([&](int i) {
        ++transform_count;
        return i * i;
    }) | views::filter([](int i) { return i % 2 == 0; });
    assert(ranges::equal(v, (int[]){4, 16}));
    assert(transform_count == 7);

    transform_count = 0;
    auto w = xs | views::transform([&](int i) {
        ++transform_count;
        return i * i;
    }) | rxx::views::cache_latest |
        views::filter([](int i) { return i % 2 == 0; });
    assert(ranges::equal(w, (int[]){4, 16}));
    assert(transform_count == 5);

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    test01();
    test02();
}
