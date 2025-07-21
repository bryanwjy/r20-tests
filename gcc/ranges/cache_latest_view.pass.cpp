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

#include "rxx/ranges/cache_latest_view.h"

#include "rxx/algorithm.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/iota_view.h"
#include "rxx/ranges/transform_view.h"

#include <cassert>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    int xs[] = {1, 2, 3, 4, 5};
    auto v = xs | xviews::cache_latest;
    assert(xranges::equal(v, xs));
    assert(xranges::size(v) == 5);

    auto it = v.begin();
    auto st = v.end();
    assert(st - it == 5);
    assert(it - st == -5);
    it++;
    assert(st - it == 4);
    assert(it - st == -4);

    auto jt = v.begin();
    xranges::iter_swap(it, jt);
    assert(xranges::equal(xs, (int[]){2, 1, 3, 4, 5}));
    int n = xranges::iter_move(it);
    assert(n == 1);
    xranges::iter_swap(it, jt);

    auto w = xviews::iota(1, 6) | xviews::cache_latest;
    assert(xranges::equal(w, xs));

    return true;
}

constexpr bool test02() {
    // Motivating example from P3138R5
    int xs[] = {1, 2, 3, 4, 5};
    int transform_count = 0;
    auto v = xs | xviews::transform([&](int i) {
        ++transform_count;
        return i * i;
    }) | xviews::filter([](int i) { return i % 2 == 0; });
    assert(xranges::equal(v, (int[]){4, 16}));
    assert(transform_count == 7);

    transform_count = 0;
    auto w = xs | xviews::transform([&](int i) {
        ++transform_count;
        return i * i;
    }) | xviews::cache_latest |
        xviews::filter([](int i) { return i % 2 == 0; });
    assert(xranges::equal(w, (int[]){4, 16}));
    assert(transform_count == 5);

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    test01();
    test02();
}
