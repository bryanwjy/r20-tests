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

#include "rxx/ranges/zip_view.h"

#include "rxx/algorithm.h"
#include "rxx/iterator.h"
#include "rxx/ranges/elements_view.h"
#include "rxx/ranges/get_element.h"
#include "rxx/ranges/iota_view.h"
#include "rxx/type_traits/common_reference.h"

#include <cassert>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::ranges::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::ranges::views;

constexpr bool test01() {
    static_assert(xranges::empty(xviews::zip()));
    static_assert(xranges::empty(xviews::empty<int>));

    auto z1 = xviews::zip(std::array{1, 2});
    auto const i0 = z1.begin(), i1 = z1.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < z1.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(z1.end() - i1 == 1);
    assert(i1 - z1.end() == -1);
    xranges::iter_swap(i0, i1);
    assert(xranges::equal(std::move(z1) | xviews::keys, (int[]){2, 1}));

    auto z2 = xviews::zip(std::array{1, 2}, std::array{3, 4, 5});
    auto i2 = z2.begin();
    i2 += 1;
    i2 -= -1;
    assert(i2 == z2.end());
    assert(xranges::size(z2) == 2);
    assert(xranges::size(std::as_const(z2)) == 2);
    assert(xranges::get_element<0>(z2[0]) == 1 &&
        xranges::get_element<1>(z2[0]) == 3);
    assert(xranges::get_element<0>(z2[1]) == 2 &&
        xranges::get_element<1>(z2[1]) == 4);
    for (auto const [x, y] : z2) {
        assert(y - x == 2);
        std::swap(x, y);
    }

    int x[2] = {1, 2}, y[2] = {3, 4}, z[2] = {5, 6};
    auto const z3 = xviews::zip(x, y, z);
    assert(xranges::size(z3) == 2);
    for (int i = 0; i < xranges::size(x); i++) {
        assert(&xranges::get_element<0>(z3[i]) == &x[i]);
        assert(&xranges::get_element<1>(z3[i]) == &y[i]);
        assert(&xranges::get_element<2>(z3[i]) == &z[i]);
    }

    return true;
}

constexpr bool test02() {
    // using __gnu_test::test_forward_range;
    // using __gnu_test::test_input_range;
    // using __gnu_test::test_random_access_range;

    // using ty1 = ranges::zip_view<views::all_t<test_forward_range<int>>,
    //     views::all_t<test_random_access_range<int>>>;
    // static_assert(ranges::forward_range<ty1>);
    // static_assert(!ranges::random_access_range<ty1>);
    // static_assert(!ranges::sized_range<ty1>);

    // using ty2 = ranges::zip_view<views::all_t<test_forward_range<int>>,
    //     views::all_t<test_input_range<int>>,
    //     views::all_t<test_forward_range<int>>>;
    // static_assert(ranges::input_range<ty2>);
    // static_assert(!ranges::forward_range<ty2>);
    // static_assert(!ranges::sized_range<ty2>);

    return true;
}

constexpr bool test03() {
    int u[] = {1, 2, 3, 4}, v[] = {4, 5, 6}, w[] = {7, 8, 9, 10};
    auto z =
        rxx::views::zip(u | views::filter([](auto) { return true; }), v, w);
    using ty = decltype(z);
    static_assert(xranges::forward_range<ty>);
    static_assert(!xranges::common_range<ty>);
    static_assert(!xranges::sized_range<ty>);
    assert(z.begin() == z.begin());
    assert(z.begin() != z.end());
    assert(xranges::next(z.begin(), 3) == z.end());
    auto it = z.begin();
    ++it;
    it++;
    it--;
    --it;
    assert(it == z.begin());

    return true;
}

constexpr bool test04() {
    // PR libstdc++/106766
#if __SIZEOF_INT128__ && 0
    auto r = xviews::zip(xviews::iota(__int128(0), __int128(1)));
#else
    auto r = xviews::zip(xviews::iota(0ll, 1ll));
#endif
    auto i = r.begin();
    auto s = r.end();
    assert(s - i == 1);
    assert(i + 1 - i == 1);

    return true;
}

constexpr bool test05() {
#if RXX_CXX23
    // This requies const assignment operator on tuple
    // PR libstdc++/109203
    int x[] = {1, 1, 2};
    int y[] = {2, 1, 3};
    auto r = xviews::zip(x, y);
    xranges::sort(r);
#endif

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    static_assert(test04());
    static_assert(test05());
}
