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

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <utility>

namespace ranges = std::ranges;
namespace views = std::views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5, 6, 7};

    auto v2 = x | rxx::views::stride(2);
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
    assert(ranges::size(v2) == 4);
    assert(ranges::equal(v2, (int[]){1, 3, 5, 7}));
    assert(ranges::equal(v2 | views::reverse, (int[]){7, 5, 3, 1}));
    assert(v2.stride() == 2);

    auto v1 = x | rxx::views::stride(1);
    assert(ranges::size(v1) == ranges::size(x));
    assert(ranges::equal(v1, x));
    assert(ranges::equal(v1 | views::reverse, x | views::reverse));
    assert(v1.stride() == 1);

    auto v5 = x | rxx::views::stride(5);
    assert(ranges::equal(v5, (int[]){1, 6}));
    assert(ranges::equal(v5 | views::reverse, (int[]){6, 1}));
    assert(v5.stride() == 5);

    auto v10 = x | rxx::views::stride(10);
    assert(ranges::equal(v10, (int[]){1}));
    assert(ranges::equal(v10 | views::reverse, (int[]){1}));
    assert(v10.stride() == 10);

    return true;
}

template <typename container>
void test02() {
    int x[] = {1, 2, 3, 4, 5, 6, 7, 8};
    container rx(x);
    auto v = rx | rxx::views::stride(3);
    assert(ranges::equal(v, (int[]){1, 4, 7}));
}

void test03() {
    // PR libstdc++/107313
    int x[] = {1, 2, 3, 4, 5};
    rxx::tests::test_input_range<int> rx(x);
    auto r = views::counted(rx.begin(), 4) | rxx::views::stride(2);
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
