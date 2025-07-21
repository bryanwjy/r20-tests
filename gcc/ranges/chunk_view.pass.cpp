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

#include "rxx/ranges/chunk_view.h"

#include "../test_iterators.h"
#include "rxx/ranges/join_view.h"
#include "rxx/ranges/zip_view.h"

#include <cassert>
#include <sstream>
#include <vector>

namespace ranges = std::ranges;
namespace views = rxx::views;

constexpr bool test01() {
    int x[] = {1, 2, 3, 4, 5};

    auto v2 = x | views::chunk(2);
    auto const i0 = v2.begin(), i1 = v2.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < v2.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(v2.end() - i1 == 2);
    assert(i1 - v2.end() == -2);
    auto i2 = v2.begin();
    i2 += 2;
    i2 -= -1;
    assert(i2 == v2.end());
    assert(ranges::size(v2) == 3);
    assert(ranges::equal(v2,
        (std::initializer_list<int>[]){
            {1, 2},
            {3, 4},
            {5}
    },
        ranges::equal));

    auto v1 = x | views::chunk(1);
    assert(ranges::size(v1) == ranges::size(x));
    for (auto [r, n] : views::zip(v1, x)) {
        assert(ranges::size(r) == 1);
        assert(*r.begin() == n);
    }

    auto v5 = x | views::chunk(5);
    assert(ranges::size(v5) == 1);
    assert(ranges::equal(v5[0], (int[]){1, 2, 3, 4, 5}));

    auto v10 = x | views::chunk(10);
    assert(ranges::size(v10) == 1);
    assert(ranges::equal(v10[0], (int[]){1, 2, 3, 4, 5}));

    return true;
}

// std::views::join is only available on libc++19

template <typename wrapper>
void test02() {
    int x[] = {1, 2, 3, 4, 5, 6, 7, 8};
    wrapper rx(x);
    auto v = rx | views::chunk(3);
    auto i = ranges::begin(v);
    assert(ranges::equal(*i, (int[]){1, 2, 3}));
    ++i;
    assert(ranges::equal(*i, (int[]){4, 5, 6}));
    ++i;
    assert(ranges::equal(*i, (int[]){7, 8}));
    i++;
    assert(i == ranges::end(v));

    for (int i = 1; i <= 10; ++i)
        assert(ranges::equal(wrapper(x) | views::chunk(i) | views::join, x));
}

void test03() {
    // LWG 3851 - chunk_view::inner-iterator missing custom iter_move and
    // iter_swap
    auto ints = std::istringstream{"0 1 2 3 4"};
    std::vector<std::string> vs{"the", "quick", "brown", "fox"};
    auto r = views::zip(vs, std::views::istream<int>(ints)) | views::chunk(2) |
        views::join;
    std::vector<std::tuple<std::string, int>> res;
    ranges::copy(std::move_iterator(r.begin()), std::move_sentinel(r.end()),
        std::back_inserter(res));
    assert(vs.front().empty());
}

int main() {
    static_assert(test01());
    test02<rxx::tests::test_input_range<int>>();
    test02<rxx::tests::test_forward_range<int>>();
    test03();

    return 0;
}
