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

#include "rxx/ranges/enumerate_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/elements_view.h"
#include "rxx/ranges/get_element.h"
#include "rxx/ranges/reverse_view.h"

#include <cassert>
#include <memory>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using rxx::tests::test_bidirectional_range;
using rxx::tests::test_forward_range;
using rxx::tests::test_input_range;
using rxx::tests::test_random_access_range;

constexpr bool test01() {
    int x[] = {1, 2, 3};
    auto v = x | xviews::enumerate;

    assert(xranges::equal(v | xviews::keys, (int[]){0, 1, 2}));
    assert(xranges::equal(v | xviews::values, (int[]){1, 2, 3}));

    auto it = v.begin();
    assert(it == it);
    assert(it != it + 1);
    assert(it != v.end());

    assert(it.index() == 0);
    assert((++it).index() == 1);
    assert((++it).index() == 2);

    return true;
}

template <template <class> class Container>
void test02() {
    int x[] = {1, 2, 3};
    Container<int> rx(x);
    auto v = rx | xviews::enumerate;

    int j = 0;
    for (auto [i, y] : v) {
        assert(&y == &x[j]);
        assert(j == i);
        ++j;
    }
    assert(j == xranges::size(x));

    if constexpr (xranges::bidirectional_range<decltype(rx)>) {
        static_assert(xranges::bidirectional_range<decltype(v)>);
        for (auto [i, y] : v | xviews::reverse) {
            --j;
            assert(&y == &x[j]);
            assert(j == i);
        }
        assert(j == 0);
    }

    if constexpr (xranges::random_access_range<decltype(rx)>) {
        static_assert(xranges::random_access_range<decltype(v)>);
        for (j = 0; j < xranges::ssize(x); ++j) {
            assert(xranges::get_element<0>(v[j]) == j);
            assert(&xranges::get_element<1>(v[j]) == &x[j]);
            assert(*(v.begin() + j) == v[j]);
            assert(*(v.begin() + (xranges::size(x) - 1) - j) ==
                v[xranges::size(x) - 1 - j]);
            assert(v.begin() + j + 1 > v.begin() + j);
            assert(v.begin() + j < v.begin() + j + 1);
            assert(v.begin() + j >= v.begin());
            assert(v.begin() <= v.begin() + j);
            assert(v.begin() + j != v.end());
            assert(v.begin() + j - v.begin() == j);
            assert(v.end() - (v.begin() + j) == xranges::ssize(x) - j);
        }
        assert(v.begin() + j == v.end());
    }
}

void test_lwg3912() {
    int x[] = {1, 2, 3};
    test_input_range<int> rx(x);
    auto v = rx | xviews::enumerate;
    auto iter = xranges::begin(v);
    // LWG 3912. enumerate_view::iterator::operator- should be noexcept
    static_assert(noexcept(iter - iter));
}

int main() {
    static_assert(test01());

    test02<test_input_range>();
    test02<test_forward_range>();
    test02<test_bidirectional_range>();
    test02<test_random_access_range>();
}