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

#include "rxx/algorithm/find_last.h"

#include "../test_iterators.h"
#include "rxx/algorithm/equal.h"
#include "rxx/ranges.h"

namespace xranges = rxx::ranges;

constexpr bool test01() {
    int x[] = {1, 2, 1, 2, 1, 2, 1, 2};

    auto sr0 = xranges::find_last(x, 0);
    assert(xranges::empty(sr0));
    assert(sr0.begin() == xranges::end(x));

    auto sr1 = xranges::find_last(x, 1);
    assert(xranges::equal(sr1, (int[]){1, 2}));
    assert(sr1.begin() == &x[6]);

    auto sr2 = xranges::find_last(x, 2);
    assert(xranges::equal(sr2, (int[]){2}));
    assert(sr2.begin() == &x[7]);

    auto plus3 = [](int n) { return n + 3; };

    auto sr3 = xranges::find_last(x, 3, plus3);
    assert(xranges::empty(sr3));
    assert(sr3.begin() == xranges::end(x));

    auto sr4 = xranges::find_last(x, 4, plus3);
    assert(xranges::equal(sr4, (int[]){1, 2}));
    assert(sr4.begin() == &x[6]);

    auto sr5 = xranges::find_last(x, 5, plus3);
    assert(xranges::equal(sr5, (int[]){2}));
    assert(sr5.begin() == &x[7]);

    return true;
}

void test02() {
    int x[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
    rxx::tests::test_forward_range<int> rx(x);

    auto sr0 = xranges::find_last(rx, 0);
    assert(xranges::empty(sr0));
    assert(sr0.begin() == xranges::end(rx));

    auto sr1 = xranges::find_last(rx, 1);
    assert(xranges::equal(sr1, (int[]){1, 2, 3}));
    assert(sr1.begin().ptr == &x[6]);

    auto sr2 = xranges::find_last(rx, 2);
    assert(xranges::equal(sr2, (int[]){2, 3}));
    assert(sr2.begin().ptr == &x[7]);

    auto sr3 = xranges::find_last(rx, 3);
    assert(xranges::equal(sr3, (int[]){3}));
    assert(sr3.begin().ptr == &x[8]);

    auto plus4 = [](int n) { return n + 4; };

    auto sr4 = xranges::find_last(rx, 4, plus4);
    assert(xranges::empty(sr4));
    assert(sr4.begin() == xranges::end(rx));

    auto sr5 = xranges::find_last(rx, 5, plus4);
    assert(xranges::equal(sr5, (int[]){1, 2, 3}));
    assert(sr5.begin().ptr == &x[6]);

    auto sr6 = xranges::find_last(rx, 6, plus4);
    assert(xranges::equal(sr6, (int[]){2, 3}));
    assert(sr6.begin().ptr == &x[7]);

    auto sr7 = xranges::find_last(rx, 7, plus4);
    assert(xranges::equal(sr7, (int[]){3}));
    assert(sr7.begin().ptr == &x[8]);
}

int main() {
    static_assert(test01());
    test02();
}
