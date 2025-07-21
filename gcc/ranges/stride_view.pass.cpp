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

#include "rxx/ranges/stride_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"

#include <array>
#include <cassert>
#include <ranges>
#include <utility>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5, 6, 7};

    auto v2 = x | xviews::stride(2);
    auto const i0 = v2.begin(), i1 = v2.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 != i1);
    assert(i0 < i1);
    assert(i0 <= i0);
    assert(i0 >= i0);
    assert(v2.end() > i1);
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(v2.end() - i1 == 3);
    assert(i1 - v2.end() == -3);
    auto i2 = v2.begin();
    i2 += 2;
    i2 -= -2;
    assert(i2 == v2.end());
    assert(xranges::size(v2) == 4);
    assert(xranges::equal(v2, (int[]){1, 3, 5, 7}));
    assert(xranges::equal(v2 | xviews::reverse, (int[]){7, 5, 3, 1}));
    assert(v2.stride() == 2);

    auto v1 = x | xviews::stride(1);
    assert(xranges::size(v1) == xranges::size(x));
    assert(xranges::equal(v1, x));
    assert(xranges::equal(v1 | xviews::reverse, x | xviews::reverse));
    assert(v1.stride() == 1);

    auto v5 = x | xviews::stride(5);
    assert(xranges::equal(v5, (int[]){1, 6}));
    assert(xranges::equal(v5 | xviews::reverse, (int[]){6, 1}));
    assert(v5.stride() == 5);

    auto v10 = x | xviews::stride(10);
    assert(xranges::equal(v10, (int[]){1}));
    assert(xranges::equal(v10 | xviews::reverse, (int[]){1}));
    assert(v10.stride() == 10);

    return true;
}

template <typename container>
void test02() {
    int x[] = {1, 2, 3, 4, 5, 6, 7, 8};
    container rx(x);
    auto v = rx | xviews::stride(3);
    assert(xranges::equal(v, (int[]){1, 4, 7}));
}

void test03() {
    // PR libstdc++/107313
    int x[] = {1, 2, 3, 4, 5};
    rxx::tests::test_input_range<int> rx(x);
    auto r = xviews::counted(rx.begin(), 4) | xviews::stride(2);
    auto i = r.begin();
    std::default_sentinel_t s = r.end();
    assert(s != i);
    assert(s - i == 2 && i - s == -2);
    ++i;
    assert(s != i);
    assert(s - i == 1 && i - s == -1);
    ++i;
    assert(s == i);
    assert(s - i == 0 && i - s == 0);
}

int main() {
    static_assert(test01());
    test02<rxx::tests::test_input_range<int>>();
    test02<rxx::tests::test_forward_range<int>>();
    test03();
}
