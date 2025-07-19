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

#include "rxx/algorithm/contains.h"

#include "../test_iterators.h"

namespace xranges = rxx::ranges;

void test01() {
    int x[] = {1, 2, 3};
    using to_input = rxx::tests::test_input_range<int>;
    assert(xranges::contains(to_input(x), 1));
    assert(xranges::contains(to_input(x), 2));
    assert(xranges::contains(to_input(x), 3));
    assert(!xranges::contains(to_input(x), 4));
    assert(!xranges::contains(x, x + 2, 3));
    auto neg = [](int n) { return -n; };
    assert(xranges::contains(to_input(x), -1, neg));
    assert(xranges::contains(to_input(x), -2, neg));
    assert(xranges::contains(to_input(x), -3, neg));
    assert(!xranges::contains(to_input(x), -4, neg));

    assert(!xranges::contains(x, x + 2, -3, neg));
}

int main() {
    test01();
}
