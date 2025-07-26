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

#include "rxx/ranges/adjacent_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/elements_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/iota_view.h"
#include "rxx/ranges/lazy_split_view.h"
#include "rxx/ranges/single_view.h"

#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    static_assert(xranges::empty(std::array{1, 2, 3} | xviews::adjacent<0>));

    auto v1 = std::array{1, 2} | xviews::adjacent<1>;
    auto const i0 = v1.begin(), i1 = v1.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < v1.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(v1.end() - i1 == 1);
    assert(i1 - v1.end() == -1);
    xranges::iter_swap(i0, i1);
    assert(xranges::equal(std::move(v1) | xviews::keys, (int[]){2, 1}));

    int x[] = {1, 2, 3, 4};
    auto v2 = x | xviews::pairwise;
    auto i2 = v2.begin();
    i2 += 2;
    i2 -= -1;
    assert(i2 == v2.end());
    assert(xranges::size(v2) == 3);
    assert(xranges::size(std::as_const(v2)) == 3);
    assert(xranges::equal(v2 | xviews::keys, (int[]){1, 2, 3}));
    assert(xranges::equal(v2 | xviews::values, (int[]){2, 3, 4}));

    int y[] = {1, 2, 3, 4, 5};
    auto const v3 = y | xviews::adjacent<3>;
    assert(xranges::size(v3) == 3);
    for (unsigned i = 0; i < xranges::size(x); i++) {
        assert(&xranges::get_element<0>(v3[i]) == &y[i] + 0);
        assert(&xranges::get_element<1>(v3[i]) == &y[i] + 1);
        assert(&xranges::get_element<2>(v3[i]) == &y[i] + 2);
    }

    // LWG 3848 - adjacent_view etc missing base accessor
    (void)v3.base();

    auto const v5 = y | xviews::adjacent<5>;
    assert(xranges::equal(v5, xviews::single(rxx::make_tuple(1, 2, 3, 4, 5))));

    auto const v6 = y | xviews::adjacent<6>;
    assert(xranges::empty(v6));

    auto const v0 = y | xviews::adjacent<0>;
    assert(xranges::empty(v0));

    return true;
}

constexpr bool test02() {
    using rxx::tests::test_forward_range;
    using rxx::tests::test_input_range;
    using rxx::tests::test_random_access_range;

    using ty1 =
        xranges::adjacent_view<xviews::all_t<test_forward_range<int>>, 2>;
    static_assert(xranges::forward_range<ty1>);
    static_assert(!xranges::bidirectional_range<ty1>);
    static_assert(!xranges::sized_range<ty1>);

    using ty2 =
        xranges::adjacent_view<xviews::all_t<test_random_access_range<int>>, 3>;
    static_assert(xranges::random_access_range<ty2>);
    static_assert(xranges::sized_range<ty2>);

    return true;
}

constexpr bool test03() {
    auto v = xviews::iota(0, 4) | xviews::filter([](auto) { return true; }) |
        xviews::pairwise;
    using ty = decltype(v);
    static_assert(xranges::forward_range<ty>);
    static_assert(xranges::common_range<ty>);
    static_assert(!xranges::sized_range<ty>);
    assert(v.begin() == v.begin());
    assert(v.begin() != v.end());
    assert(xranges::next(v.begin(), 3) == v.end());
    auto it = v.begin();
    ++it;
    it++;
    assert(xranges::next(it) == v.end());
    it--;
    --it;
    assert(it == v.begin());

    return true;
}

constexpr bool test04() {
    // PR libstdc++/106798
    auto r = xviews::single(0) | xviews::lazy_split(0) | xviews::pairwise;
    decltype(xranges::cend(r)) s = r.end();
    // TODO: lazy split view has a bug
    // assert(r.begin() != s);

    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    static_assert(test04());
}