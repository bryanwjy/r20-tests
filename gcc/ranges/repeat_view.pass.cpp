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

#include "rxx/ranges/repeat_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/drop_view.h"
#include "rxx/ranges/take_view.h"

#include <cassert>
#include <memory>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test01() {
    auto v = xviews::repeat(42);
    static_assert(xranges::random_access_range<decltype(v)> &&
        !xranges::sized_range<decltype(v)>);
    auto i = xranges::begin(v);
    auto s = xranges::end(v);
    assert(*i == 42);
    assert(i[0] == 42);
    assert(&i[0] == &i[1]);
    assert(&*i == &*(i + 1));
    assert(i != s);
    auto j = i + 5, k = i + 12;
    assert(k - i == 12);
    assert(k - j == 7);
    assert(i - j == -5);
    assert(k > j);
    assert(j < k);
    assert(i + 5 == j);
    assert(i != j);
    assert(i + 5 <= j);
    assert(j - 5 >= i);

    return true;
}

constexpr bool test02() {
    constexpr int bound = 20;
    auto v = xviews::repeat(42, bound);
    static_assert(xranges::random_access_range<decltype(v)> &&
        xranges::sized_range<decltype(v)>);
    assert(xranges::equal(v, xviews::repeat(42) | xviews::take(bound)));
    auto i = xranges::begin(v);
    auto s = xranges::end(v);
    assert(*i == 42);
    assert(i[0] == 42);
    assert(&i[0] == &i[1]);
    assert(&*i == &*(i + 1));
    assert(i != s);
    auto j = i + 5, k = i + 12;
    assert(k - i == 12);
    assert(k - j == 7);
    assert(i - j == -5);
    assert(k > j);
    assert(j < k);
    assert(i + 5 == j);
    assert(i != j);
    assert(i + 5 <= j);
    assert(j - 5 >= i);

    assert(xranges::size(v) == bound);
    assert(s - i == bound);
    assert(s - j == bound - (j - i));
    assert(i + bound == s);
    assert(bound + i == s);

    return true;
}

constexpr bool test03() {
    struct A {
        int n, m;
    };
#if !RXX_TARGET_APPLE
    auto v = xranges::repeat_view<A, unsigned>(
        std::piecewise_construct, std::tuple{1, 2}, std::tuple{3});
    assert(v[0].n == 1);
    assert(v[0].m == 2);
    assert(xranges::size(v) == 3);
#else
    // apple clang bug
    static_assert(!std::constructible_from<A, int, int>);
#endif

    return true;
}

constexpr bool test04() {
    // Verify P2474R2 changes to xviews::take/drop.
    auto r = xviews::repeat(42);

    auto rt = r | xviews::take(10);
    static_assert(xranges::details::is_repeat_view<decltype(rt)>);
    assert(xranges::equal(rt, xviews::repeat(42, 10)));

    auto rd = r | xviews::drop(10);
    static_assert(std::same_as<decltype(rd), decltype(r)>);

    auto br = xviews::repeat(42, 37);

    auto brt = br | xviews::take(10);
    static_assert(std::same_as<decltype(brt), decltype(br)>);
    assert(xranges::equal(brt, xviews::repeat(42, 10)));

    auto brt100 = br | xviews::take(100);
    assert(xranges::equal(brt100, br));

    auto brd = br | xviews::drop(10);
    static_assert(std::same_as<decltype(brd), decltype(br)>);
    assert(xranges::equal(brd, xviews::repeat(42, 27)));

    auto brd100 = br | xviews::drop(100);
    assert(xranges::empty(brd100));

    return true;
}

void test05() {
    // LWG 3796
    xranges::repeat_view<int> r;
}

void test06() {
    struct move_only {
        move_only() {}
        move_only(move_only&&) {}
    };
    // P2494R2 Relaxing range adaptors to allow for move only types
    static_assert(requires { xviews::repeat(move_only{}, 2); });
}

void test07() {
    // PR libstdc++/112453
    auto t1 = xviews::repeat(std::make_unique<int>(5)) | xviews::take(2);
    auto d1 = xviews::repeat(std::make_unique<int>(5)) | xviews::drop(2);

    auto t2 = xviews::repeat(std::make_unique<int>(5), 4) | xviews::take(2);
    auto d2 = xviews::repeat(std::make_unique<int>(5), 4) | xviews::drop(2);
}

void test08() {
    // LWG 4053 - Unary call to xviews::repeat does not decay the argument
    using type = xranges::repeat_view<char const*>;
    using type = decltype(xviews::repeat("foo", std::unreachable_sentinel));
    using type = decltype(xviews::repeat(+"foo", std::unreachable_sentinel));
    using type = decltype(xviews::repeat("foo"));
    using type = decltype(xviews::repeat(+"foo"));
}

void test09() {
    // LWG 4054 - Repeating a repeat_view should repeat the view
    auto v = xviews::repeat(xviews::repeat(5));
    using type = decltype(v);
    using type = xranges::repeat_view<xranges::repeat_view<int>>;
    assert(v[0][0] == 5);
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    static_assert(test04());
    test05();
    test06();
    test07();
    test08();
    test09();
}
