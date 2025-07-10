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

#include "rxx/adjacent_transform_view.h"

#include "test_iterators.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    auto v1 =
        std::array{1, 2, 3} | xviews::adjacent_transform<1>(std::identity{});
    assert(ranges::equal(v1, (int[]){1, 2, 3}));
    auto const i0 = v1.begin(), i1 = v1.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < v1.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(v1.end() - i1 == 2);
    assert(i1 - v1.end() == -2);
    ranges::iter_swap(i0, i1);
    assert(ranges::equal(std::move(v1), (int[]){2, 1, 3}));

    auto v2 = std::array{1, -1, 2, -2} |
        xviews::pairwise_transform(std::multiplies{});
    auto i2 = v2.begin();
    i2 += 1;
    i2 -= -2;
    assert(i2 == v2.end());
    assert(xranges::size(v2) == 3);
    assert(xranges::size(std::as_const(v2)) == 3);
    assert(ranges::equal(v2, (int[]){-1, -2, -4}));

    int y[] = {1, 2, 3, 4, 5, 6};
    auto v3 = y | xviews::adjacent_transform<3>([](auto... xs) {
        return ranges::max({xs...});
    });
    assert(xranges::size(v3) == 4);
    assert(ranges::equal(v3, (int[]){3, 4, 5, 6}));

    // LWG 3848 - adjacent_transform_view etc missing base accessor
    (void)v3.base();

    auto const v6 =
        y | xviews::adjacent_transform<6>([](auto...) { return 0; });
    assert(ranges::equal(v6, (int[]){0}));

    auto const v7 =
        y | xviews::adjacent_transform<7>([](auto...) { return 0; });
    assert(ranges::empty(v7));

    auto const v0 = y | xviews::adjacent_transform<0>([] { return 0; });
    assert(ranges::empty(v0));

    return true;
}

constexpr bool test02() {
    using rxx::tests::test_forward_range;
    using rxx::tests::test_input_range;
    using rxx::tests::test_random_access_range;

    using ty1 =
        xranges::adjacent_transform_view<views::all_t<test_forward_range<int>>,
            std::plus<>, 2>;
    static_assert(ranges::forward_range<ty1>);
    static_assert(!xranges::bidirectional_range<ty1>);
    static_assert(!xranges::sized_range<ty1>);

    using ty2 = xranges::adjacent_transform_view<
        views::all_t<test_random_access_range<int>>,
        decltype([](int, int, int) { return 0; }), 3>;
    static_assert(xranges::random_access_range<ty2>);
    static_assert(xranges::sized_range<ty2>);

    return true;
}

constexpr bool test03() {
    auto v = views::iota(0, 4) | views::filter([](auto) { return true; }) |
        xviews::pairwise_transform(std::plus{});
    using ty = decltype(v);
    static_assert(xranges::forward_range<ty>);
    static_assert(xranges::common_range<ty>);
    static_assert(!xranges::sized_range<ty>);
    assert(v.begin() == v.begin());
    assert(v.begin() != v.end());
    assert(ranges::next(v.begin(), 3) == v.end());
    auto it = v.begin();
    ++it;
    it++;
    assert(ranges::next(it) == v.end());
    it--;
    --it;
    assert(it == v.begin());

    return true;
}

void test04() {
    extern int x[5];
    struct move_only {
        move_only() {}
        move_only(move_only&&) {}
        int operator()(int i, int j) const { return i + j; }
    };
    // P2494R2 Relaxing range adaptors to allow for move only types
    static_assert(requires { xviews::pairwise_transform(x, move_only{}); });
    static_assert(requires { x | xviews::pairwise_transform(move_only{}); });
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    test04();
}