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

#include "rxx/ranges/chunk_by_view.h"

#include "rxx/ranges/join_view.h"
#include "test_iterators.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <sstream>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;

constexpr bool test01() {
    int x[] = {1, 2, 2, 3, 0, 4, 5, 2};
    auto v = x | rxx::views::chunk_by(ranges::less_equal{});
    static_assert(ranges::bidirectional_range<decltype(v)> &&
        ranges::common_range<decltype(v)>);
    assert(ranges::equal(v,
        (std::initializer_list<int>[]){
            {1, 2, 2, 3},
            {0, 4, 5},
            {2}
    },
        ranges::equal));
    assert(ranges::equal(v | views::reverse,
        (std::initializer_list<int>[]){
            {2},
            {0, 4, 5},
            {1, 2, 2, 3}
    },
        ranges::equal));
    assert(ranges::equal(v | rxx::views::join, x));
    auto i = v.begin();
    auto j = i;
    j++;
    assert(i == i && i != v.end());
    assert(j == j && j != v.end());
    assert(j != i);
    j--;
    assert(j == i);

    return true;
}

void test02() {
    int x[] = {1, 2, 3};
    rxx::tests::test_forward_range<int> rx(x);
    auto v = rx | rxx::views::chunk_by(ranges::equal_to{});
    static_assert(!ranges::bidirectional_range<decltype(v)> &&
        !ranges::common_range<decltype(v)>);
    assert(
        ranges::equal(v, x | views::transform(views::single), ranges::equal));
    auto i = v.begin();
    assert(i != v.end());
    ranges::advance(i, 3);
    assert(i == v.end());
}

void test03() {
    // LWG 3796
    rxx::ranges::chunk_by_view<ranges::empty_view<int>, ranges::equal_to> r;
}

constexpr bool test04() {
    // PR libstdc++/108291
    using namespace std::literals;
    std::string_view s = "hello";
    auto r = s | rxx::views::chunk_by(std::less{});
    assert(ranges::equal(
        r, (std::string_view[]){"h"sv, "el"sv, "lo"sv}, ranges::equal));
    assert(ranges::equal(r | views::reverse,
        (std::string_view[]){"lo"sv, "el"sv, "h"sv}, ranges::equal));

    return true;
}

void test05() {
    // PR libstdc++/109474
    std::vector<bool> v = {true, false, true, true, false, false};
    auto r = v | rxx::views::chunk_by(std::equal_to{});
    assert(ranges::equal(r,
        (std::initializer_list<bool>[]){
            {true},
            {false},
            {true, true},
            {false, false}
    },
        ranges::equal));
    assert(ranges::equal(r | views::reverse,
        (std::initializer_list<bool>[]){
            {false, false},
            {true, true},
            {false},
            {true}
    },
        ranges::equal));
}

int main() {
    static_assert(test01());
    test02();
    test03();
    static_assert(test04());
    test05();
}
