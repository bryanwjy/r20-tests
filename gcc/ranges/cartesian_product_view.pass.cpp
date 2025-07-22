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

#include "rxx/ranges/cartesian_product_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/counted.h"
#include "rxx/ranges/elements_view.h"
#include "rxx/ranges/repeat_view.h"
#include "rxx/ranges/reverse_view.h"

#include <cassert>
#include <ranges>
#include <sstream>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    int x[] = {1, 2, 3};
    int y[] = {4, 5, 6};
    int z[] = {7, 8};
    int w[] = {9};

    auto v0 = xviews::cartesian_product();
    assert(xranges::end(v0) - xranges::begin(v0) == 1);
    assert(xranges::size(v0) == 1);
    assert(!xranges::empty(v0));

    auto v1 = xviews::cartesian_product(x);
    assert(xranges::end(v1) - xranges::begin(v1) == 3);
    assert(xranges::size(v1) == 3);
    assert(xranges::equal(v1 | xviews::keys, x));
    assert(xranges::get_element<0>(v1[0]) == 1);
    assert(xranges::get_element<0>(v1[1]) == 2);
    assert(xranges::get_element<0>(v1[2]) == 3);
    assert(xranges::equal(
        v1 | xviews::reverse | xviews::keys, x | xviews::reverse));

    auto v2 = xviews::cartesian_product(x, y);
    assert(xranges::size(v2) == 9);
    assert(xranges::end(v2) - xranges::begin(v2) == 9);
    assert(
        xranges::equal(v2 | xviews::keys, (int[]){1, 1, 1, 2, 2, 2, 3, 3, 3}));
    assert(xranges::equal(
        v2 | xviews::values, (int[]){4, 5, 6, 4, 5, 6, 4, 5, 6}));
    assert(xranges::equal(v2 | xviews::reverse | xviews::keys,
        (int[]){3, 3, 3, 2, 2, 2, 1, 1, 1}));
    assert(xranges::equal(v2 | xviews::reverse | xviews::values,
        (int[]){6, 5, 4, 6, 5, 4, 6, 5, 4}));

    auto v3 = xviews::cartesian_product(x, y, z);
    assert(xranges::size(v3) == 18);
    assert(xranges::equal(v3,
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

    auto v4 = xviews::cartesian_product(x, y, z, w);
    assert(xranges::size(v4) == 18);
    assert(xranges::equal(v4 | xviews::elements<3>, xviews::repeat(9, 18)));

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
    xranges::iter_swap(i4, j4);
    assert(*j4 == std::tuple(1, 5, 7, 9));
    assert(*i4 == std::tuple(1, 4, 8, 9));

    return true;
}

void test02() {
    int x[] = {1, 2};
    rxx::tests::test_input_range<int> rx(x);
    auto v = xviews::cartesian_product(rx, x);
    auto i = v.begin();
    std::default_sentinel_t s = v.end();
    assert(i != s);
    assert(
        xranges::get_element<0>(*i) == 1 && xranges::get_element<1>(*i) == 1);
    ++i;
    assert(i != s);
    assert(
        xranges::get_element<0>(*i) == 1 && xranges::get_element<1>(*i) == 2);
    ++i;
    assert(i != s);
    assert(
        xranges::get_element<0>(*i) == 2 && xranges::get_element<1>(*i) == 1);
    ++i;
    assert(i != s);
    assert(
        xranges::get_element<0>(*i) == 2 && xranges::get_element<1>(*i) == 2);
    ++i;
    assert(i == s);
}

void test03() {
    int x[2];
    rxx::tests::test_input_range<int> rx(x);
    auto v = xviews::cartesian_product(xviews::counted(rx.begin(), 2), x);
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
    auto v = xviews::cartesian_product(x, y, z, w);

    auto n = xranges::ssize(v);
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
#if RXX_SUPPORTS_INT128 && RXX_LIBSTDCXX_AT_LEAST(16)
    auto r = xviews::iota(__int128(0), __int128(5));
#else
    auto r = xviews::iota(0ll, 5ll);
#endif
    auto v = xviews::cartesian_product(r, r);
    assert(xranges::size(v) == 25);
    assert(v.end() - v.begin() == 25);
    assert(v.begin() + xranges::ssize(v) - v.begin() == 25);
}

constexpr bool test06() {
    int x[] = {1, 2, 3};
    auto v = xviews::cartesian_product(x, xviews::empty<int>, x);
    assert(xranges::size(v) == 0);
    assert(xranges::begin(v) == xranges::end(v));
    assert(xranges::begin(v) - xranges::begin(v) == 0);

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
    static_assert(!xranges::forward_range<istream_range> &&
        xranges::common_range<istream_range>);
    istream_range r;
    int i = 0;
    for (auto [v] : xviews::cartesian_product(r)) {
        assert(v == i);
        ++i;
    };
    assert(i == 5);
}

void test08() {
    // LWG 3820
    auto r = xviews::cartesian_product(xviews::iota(0));
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
