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

#include "rxx/ranges/cartesian_product_view.h"

#include "rxx/ranges/repeat_view.h"
#include "test_iterators.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <sstream>

namespace ranges = std::ranges;
namespace views = std::views;

constexpr bool test01() {
    int x[] = {1, 2, 3};
    int y[] = {4, 5, 6};
    int z[] = {7, 8};
    int w[] = {9};

    auto v0 = rxx::views::cartesian_product();
    assert(ranges::end(v0) - ranges::begin(v0) == 1);
    assert(ranges::size(v0) == 1);
    assert(!ranges::empty(v0));

    auto v1 = rxx::views::cartesian_product(x);
    assert(ranges::end(v1) - ranges::begin(v1) == 3);
    assert(ranges::size(v1) == 3);
    assert(ranges::equal(v1 | views::keys, x));
    assert(std::get<0>(v1[0]) == 1);
    assert(std::get<0>(v1[1]) == 2);
    assert(std::get<0>(v1[2]) == 3);
    assert(
        ranges::equal(v1 | views::reverse | views::keys, x | views::reverse));

    auto v2 = rxx::views::cartesian_product(x, y);
    assert(ranges::size(v2) == 9);
    assert(ranges::end(v2) - ranges::begin(v2) == 9);
    assert(ranges::equal(v2 | views::keys, (int[]){1, 1, 1, 2, 2, 2, 3, 3, 3}));
    assert(
        ranges::equal(v2 | views::values, (int[]){4, 5, 6, 4, 5, 6, 4, 5, 6}));
    assert(ranges::equal(
        v2 | views::reverse | views::keys, (int[]){3, 3, 3, 2, 2, 2, 1, 1, 1}));
    assert(ranges::equal(v2 | views::reverse | views::values,
        (int[]){6, 5, 4, 6, 5, 4, 6, 5, 4}));

    auto v3 = rxx::views::cartesian_product(x, y, z);
    assert(ranges::size(v3) == 18);
    assert(ranges::equal(v3,
        (std::tuple<int, int, int>[]){
            {1, 4, 7},
            {1, 4, 8},
            {1, 5, 7},
            {1, 5, 8},
            {1, 6, 7},
            {1, 6, 8},
            {2, 4, 7},
            {2, 4, 8},
            {2, 5, 7},
            {2, 5, 8},
            {2, 6, 7},
            {2, 6, 8},
            {3, 4, 7},
            {3, 4, 8},
            {3, 5, 7},
            {3, 5, 8},
            {3, 6, 7},
            {3, 6, 8}
    }));

    auto v4 = rxx::views::cartesian_product(x, y, z, w);
    assert(ranges::size(v4) == 18);
    assert(ranges::equal(v4 | views::elements<3>, rxx::views::repeat(9, 18)));

    auto i4 = v4.begin(), j4 = i4 + 1;
    assert(j4 > i4);
    assert(i4[0] == std::tuple(1, 4, 7, 9));
    assert(i4 + 18 == v4.end());
    i4 += 5;
    assert(i4 != v4.begin());
    assert(i4 - 5 == v4.begin());
    assert(*i4 == std::tuple(1, 6, 8, 9));
    assert(i4 - 5 != i4);
    i4 -= 3;
    assert(*i4 == std::tuple(1, 5, 7, 9));
    assert(j4 + 1 == i4);
    ranges::iter_swap(i4, j4);
    assert(*j4 == std::tuple(1, 5, 7, 9));
    assert(*i4 == std::tuple(1, 4, 8, 9));

    return true;
}

void test02() {
    int x[] = {1, 2};
    rxx::tests::test_input_range<int> rx(x);
    auto v = rxx::views::cartesian_product(rx, x);
    auto i = v.begin();
    std::default_sentinel_t s = v.end();
    assert(i != s);
    assert(std::get<0>(*i) == 1 && std::get<1>(*i) == 1);
    ++i;
    assert(i != s);
    assert(std::get<0>(*i) == 1 && std::get<1>(*i) == 2);
    ++i;
    assert(i != s);
    assert(std::get<0>(*i) == 2 && std::get<1>(*i) == 1);
    ++i;
    assert(i != s);
    assert(std::get<0>(*i) == 2 && std::get<1>(*i) == 2);
    ++i;
    assert(i == s);
}

void test03() {
    int x[2];
    rxx::tests::test_input_range<int> rx(x);
    auto v = rxx::views::cartesian_product(views::counted(rx.begin(), 2), x);
    assert(v.size() == 4);
    auto i = v.begin();
    std::default_sentinel_t s = v.end();
    assert(i - s == -4);
    assert(s - i == 4);
    ++i;
    assert(i - s == -3);
    assert(s - i == 3);
    ++i;
    assert(i - s == -2);
    assert(s - i == 2);
    ++i;
    assert(i - s == -1);
    assert(s - i == 1);
    ++i;
    assert(i - s == 0);
    assert(s - i == 0);
}

void test04() {
    // Exhaustively verify correctness of our iterator addition implementation
    // (which runs in constant time) for this 24-element cartesian_product_view.
    int x[4], y[3], z[2], w[1];
    auto v = rxx::views::cartesian_product(x, y, z, w);

    auto n = ranges::ssize(v);
    for (int i = 0; i <= n; i++)
        for (int j = 0; i + j <= n; j++) {
            auto b1 = v.begin();
            for (int k = 0; k < i + j; k++)
                ++b1;
            assert(b1 - v.begin() == i + j);
            auto b2 = (v.begin() + i) + j;
            auto b3 = v.begin() + (i + j);
            assert(b1 == b2 && b2 == b3);

            auto e1 = v.end();
            for (int k = 0; k < i + j; k++)
                --e1;
            assert(v.end() - e1 == i + j);
            auto e2 = (v.end() - i) - j;
            auto e3 = v.end() - (i + j);
            assert(e1 == e2 && e2 == e3);
        }
}

void test05() {
    // libc++ does not support this
#if RXX_LIBSTDCXX
    auto r = views::iota(__int128(0), __int128(5));
#else
    auto r = views::iota(0ll, 5ll);
#endif
    auto v = rxx::views::cartesian_product(r, r);
    assert(ranges::size(v) == 25);
    assert(v.end() - v.begin() == 25);
    assert(v.begin() + ranges::ssize(v) - v.begin() == 25);
}

constexpr bool test06() {
    int x[] = {1, 2, 3};
    auto v = rxx::views::cartesian_product(x, views::empty<int>, x);
    assert(ranges::size(v) == 0);
    assert(ranges::begin(v) == ranges::end(v));
    assert(ranges::begin(v) - ranges::begin(v) == 0);

    return true;
}

void test07() {
    // PR libstdc++/107572
    static std::istringstream ints("0 1 2 3 4");
    struct istream_range {
        auto begin() { return std::istream_iterator<int>{ints}; }
        auto end() { return std::istream_iterator<int>{}; }
        using iterator_concept = std::input_iterator_tag;
    };
    static_assert(!ranges::forward_range<istream_range> &&
        ranges::common_range<istream_range>);
    istream_range r;
    int i = 0;
    for (auto [v] : rxx::views::cartesian_product(r)) {
        assert(v == i);
        ++i;
    };
    assert(i == 5);
}

void test08() {
    // LWG 3820
    auto r = rxx::views::cartesian_product(views::iota(0));
    r.begin() += 3;
}

int main() {
    static_assert(test01());
    test02();
    test03();
    test04();
    test05();
    static_assert(test06());
    test07();
    test08();
}
