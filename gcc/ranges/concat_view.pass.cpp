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

#include "rxx/ranges/concat_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/basic_istream_view.h"
#include "rxx/ranges/drop_view.h"
#include "rxx/ranges/empty_view.h"
#include "rxx/ranges/iota_view.h"
#include "rxx/ranges/join_view.h"
#include "rxx/ranges/reverse_view.h"
#include "rxx/ranges/single_view.h"
#include "rxx/ranges/transform_view.h"

#include <array>
#include <cassert>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    std::vector<int> v1{1, 2, 3}, v2{4, 5}, v3{};
    std::array a{6, 7, 8};
    auto s = xviews::single(9);

    auto v = xviews::concat(v1, v2, v3, a, s);

    assert(xranges::size(v) == 9);
    assert(xranges::size(std::as_const(v)) == 9);
    assert(xranges::equal(v, xviews::iota(1, 10)));
    assert(xranges::equal(
        v | xviews::reverse, xviews::iota(1, 10) | xviews::reverse));

    auto it0 = v.begin();
    auto cit = std::as_const(v).begin();
    assert(it0 == it0);
    assert(cit == cit);
    assert(it0 == cit);
    for (int i = 0; i < 10; i++) {
        assert(it0 + i - it0 == i);
        assert(it0 + i - (it0 + 1) == i - 1);
        assert(it0 + i - (it0 + 3) == i - 3);
        assert(it0 + i - (it0 + 5) == i - 5);
        assert(it0 + i - i + i == it0 + i);
        assert(it0 + i - (it0 + i) == 0);
    }
    assert(std::default_sentinel - it0 == 9);
    assert(it0 + 9 == std::default_sentinel);

    auto it5 = it0 + 5;
    xranges::iter_swap(it0, it5);
    assert(*it0 == 6 && *it5 == 1);
    xranges::iter_swap(it0, it5);
    *it0 = xranges::iter_move(it0);
    return true;
}

void test02() {
    int x[] = {1, 2, 3, 4, 5};
    rxx::tests::test_input_range<int> rx(x);
    auto v = xviews::concat(xviews::single(0), rx, xviews::empty<int>);
    static_assert(!xranges::forward_range<decltype(v)>);
    assert(xranges::equal(v | xviews::drop(1), x));
}

void test03() {
    // LWG 4166 - concat_view::end() should be more constrained in order to
    // support noncopyable iterators
    auto range_copyable_it = std::vector<int>{1, 2, 3};

    std::stringstream ss{"4 5 6"};
    auto range_noncopyable_it = xviews::istream<int>(ss);
    xranges::range auto view1 =
        xviews::concat(range_copyable_it, range_noncopyable_it);
    assert(xranges::equal(view1, std::vector{1, 2, 3, 4, 5, 6}));

    ss = std::stringstream{"4 5 6"};
    range_noncopyable_it = xviews::istream<int>(ss);
    static_assert(xranges::details::member_end<decltype(xviews::concat(
            range_noncopyable_it, range_copyable_it))&>);
    xranges::range auto view2 =
        xviews::concat(range_noncopyable_it, range_copyable_it);
    assert(xranges::equal(view2, std::vector{4, 5, 6, 1, 2, 3}));
}

void test04() {
    // PR libstdc++/115215 - xviews::concat rejects non-movable reference
    int x[] = {1, 2, 3};
    struct nomove {
        nomove() = default;
        nomove(nomove const&) = delete;
    };
    auto v = x | xviews::transform([](int) { return nomove{}; });
    using type = decltype(xviews::concat(v));
    using type = decltype(v);
}

void test05() {
    // PR libstdc++/120934 - xviews::concat is ill-formed depending on argument
    // order
    auto v1 = xviews::single(1);
    std::vector<int> vec = {2, 3};
    auto v2 = xviews::join(xviews::transform(vec, xviews::single));

    static_assert(xranges::range<decltype(xviews::concat(v1, v2))>);
    static_assert(xranges::range<decltype(xviews::concat(v2, v1))>);
}

int main() {
    static_assert(test01());
    test01();
    test02();
    test03();
    test04();
    test05();
}
