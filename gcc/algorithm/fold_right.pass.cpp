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
#include "rxx/ranges/concepts.h"
#include "rxx/ranges/filter_view.h"

#include <cassert>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5};
    auto v = x | xviews::filter([](int) { return true; });
    static_assert(xranges::bidirectional_range<decltype(v)> &&
        !xranges::random_access_range<decltype(v)>);
    auto f = [](int& x, int&& acc) { return 2 * acc + x; };
    assert(xranges::fold_right(v, 0, f) == 129);
    assert(xranges::fold_right(v, 1, f) == 161);
    assert(xranges::fold_right(v.begin(), v.begin(), 1, f) == 1);

    assert(xranges::fold_right_last(v, f).value() == 129);
    assert(!xranges::fold_right_last(v.begin(), v.begin(), f).has_value());

    return true;
}

constexpr bool test02() {
    double x[] = {0.5, 0.25, 0.125, 0.125};
    assert(xranges::fold_right(x, 0, std::plus{}) == 1.0);

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
}
