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

#include "../test_iterators.h"
#include "rxx/algorithm/fold.h"
#include "rxx/ranges/take_view.h"

#include <cassert>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5};
    auto f = [](int&& acc, int& x) { return 2 * acc + x; };
    assert(xranges::fold_left(x, 0, f) == 57);
    assert(xranges::fold_left(x, 1, f) == 89);
    assert(xranges::fold_left(x + 0, x + 0, 1, f) == 1);

    assert(xranges::fold_left_first(x, f).value() == 57);
    assert(!xranges::fold_left_first(x + 0, x + 0, f).has_value());

    return true;
}

void test02() {
    int x[] = {1, 2, 3, 4, 5};
    auto f = [](int&& acc, int& x) { return 2 * acc + x; };

    __RXX tests::test_input_range<int> rx(x);
    xranges::in_value_result ivr = xranges::fold_left_with_iter(rx, 0, f);
    assert(ivr.in == rx.end());
    assert(ivr.value == 57);

    rx.bounds.first = x;
    xranges::in_value_result ivr2 = xranges::fold_left_first_with_iter(rx, f);
    assert(ivr2.in == rx.end());
    assert(ivr2.value.value() == 57);

    rx.bounds.first = x;
    auto v = rx | xviews::take(0);
    xranges::in_value_result ivr3 = xranges::fold_left_first_with_iter(v, f);
    assert(ivr3.in == v.end());
    assert(!ivr3.value.has_value());
}

constexpr bool test03() {
    double x[] = {0.5, 0.25, 0.125, 0.125};
    assert(xranges::fold_left(x, 0, std::plus{}) == 1.0);
    assert(xranges::fold_left_with_iter(x, 0, std::plus{}).value == 1.0);

    return true;
}

int main() {
    static_assert(test01());
    test02();
    static_assert(test03());
}
