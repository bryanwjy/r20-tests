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
#include "rxx/algorithm/equal.h"
#include "rxx/numeric/iota.h"

#include <cassert>

namespace xranges = __RXX ranges;

void test01() {
    int x[3] = {};
    __RXX tests::test_output_range<int> rx(x);
    auto r0 = xranges::iota(rx, 0);
    assert(r0.out.ptr == x + 3);
    assert(r0.value == 3);
    assert(xranges::equal(x, (int[]){0, 1, 2}));
    auto r1 = xranges::iota(x, x + 2, 5);
    assert(r1.out == x + 2);
    assert(r1.value == 7);
    assert(xranges::equal(x, (int[]){5, 6, 2}));
}

int main() {
    test01();
}
