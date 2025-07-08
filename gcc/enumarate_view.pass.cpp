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

#include "rxx/enumerate_view.h"
#include "rxx/get_element.h"
#include "test_iterators.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ranges>

namespace ranges = std::ranges;
namespace views = std::views;

using rxx::tests::test_bidirectional_range;
using rxx::tests::test_forward_range;
using rxx::tests::test_input_range;
using rxx::tests::test_random_access_range;

constexpr bool test01() {
    int x[] = {1, 2, 3};
    auto v = x | rxx::views::enumerate;

    assert(ranges::equal(v | views::keys, (int[]){0, 1, 2}));
    assert(ranges::equal(v | views::values, (int[]){1, 2, 3}));

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
    auto v = rx | rxx::views::enumerate;

    int j = 0;
    for (auto [i, y] : v) {
        assert(&y == &x[j]);
        assert(j == i);
        ++j;
    }
    assert(j == ranges::size(x));

    if constexpr (ranges::bidirectional_range<decltype(rx)>) {
        static_assert(ranges::bidirectional_range<decltype(v)>);
        for (auto [i, y] : v | views::reverse) {
            --j;
            assert(&y == &x[j]);
            assert(j == i);
        }
        assert(j == 0);
    }

    if constexpr (ranges::random_access_range<decltype(rx)>) {
        static_assert(ranges::random_access_range<decltype(v)>);
        for (j = 0; j < ranges::ssize(x); ++j) {
            assert(rxx::ranges::get_element<0>(v[j]) == j);
            assert(&rxx::ranges::get_element<1>(v[j]) == &x[j]);
            assert(*(v.begin() + j) == v[j]);
            assert(*(v.begin() + (ranges::size(x) - 1) - j) ==
                v[ranges::size(x) - 1 - j]);
            assert(v.begin() + j + 1 > v.begin() + j);
            assert(v.begin() + j < v.begin() + j + 1);
            assert(v.begin() + j >= v.begin());
            assert(v.begin() <= v.begin() + j);
            assert(v.begin() + j != v.end());
            assert(v.begin() + j - v.begin() == j);
            assert(v.end() - (v.begin() + j) == ranges::ssize(x) - j);
        }
        assert(v.begin() + j == v.end());
    }
}

void test_lwg3912() {
    int x[] = {1, 2, 3};
    test_input_range<int> rx(x);
    auto v = rx | rxx::views::enumerate;
    auto iter = std::ranges::begin(v);
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
