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

#include "rxx/ranges/zip_transform_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/filter_view.h"

#include <cassert>
#include <ranges>
#include <utility>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <typename T>
concept can_zip_transform =
    requires(T t) { xviews::zip_transform(std::forward<T>(t)); };

static_assert(!can_zip_transform<int>);

struct NonMovable {
    NonMovable(NonMovable&&) = delete;
};

static_assert(!can_zip_transform<NonMovable>);
static_assert(!can_zip_transform<NonMovable&>);

static_assert(!can_zip_transform<void (*)()>);
static_assert(can_zip_transform<int (&(*)())[3]>);

constexpr bool test01() {
    static_assert(xranges::empty(xviews::zip_transform([] { return 0; })));

    auto z1 = xviews::zip_transform(std::identity{}, std::array{1, 2, 3});
    assert(xranges::equal(z1, (int[]){1, 2, 3}));
    auto const i0 = z1.begin(), i1 = z1.begin() + 1;
    assert(i0 + 1 - 1 == i0);
    assert(i0 < i1);
    assert(i1 < z1.end());
    assert(i1 - i0 == 1);
    assert(i0 - i1 == -1);
    assert(z1.end() - i1 == 2);
    assert(i1 - z1.end() == -2);
    xranges::iter_swap(i0, i1);
    assert(xranges::equal(std::move(z1), (int[]){2, 1, 3}));

    auto z2 = xviews::zip_transform(
        std::multiplies{}, std::array{-1, 2}, std::array{3, 4, 5});
    auto i2 = z2.begin();
    i2 += 1;
    i2 -= -1;
    assert(i2 == z2.end());
    assert(xranges::size(z2) == 2);
    assert(xranges::size(std::as_const(z2)) == 2);
    assert(xranges::equal(z2, (int[]){-3, 8}));

    auto z3 = xviews::zip_transform(
        [](auto... xs) { return xranges::max({xs...}); },
        std::array{1, 6, 7, 0, 0}, std::array{2, 5, 9}, std::array{3, 4, 8, 0});
    assert(xranges::size(z3) == 3);
    assert(xranges::equal(z3, (int[]){3, 6, 9}));

    auto z4 = xviews::zip_transform([]() { return 1; });
    assert(xranges::size(z4) == 0);
    static_assert(std::same_as<xranges::range_value_t<decltype(z4)>, int>);

    return true;
}

constexpr bool test02() {
    using rxx::tests::test_forward_range;
    using rxx::tests::test_input_range;
    using rxx::tests::test_random_access_range;

    using ty1 = xranges::zip_transform_view<std::plus<>,
        xviews::all_t<test_forward_range<int>>,
        xviews::all_t<test_random_access_range<int>>>;
    static_assert(
        std::sentinel_for<decltype(__RXX_AUTOCAST(std::declval<ty1&>().end())),
            xranges::iterator_t<ty1>>);
    static_assert(xranges::forward_range<ty1>);
    static_assert(!xranges::random_access_range<ty1>);
    static_assert(!xranges::sized_range<ty1>);

    using ty2 =
        xranges::zip_transform_view<decltype([](int, int, int) { return 0; }),
            xviews::all_t<test_forward_range<int>>,
            xviews::all_t<test_input_range<int>>,
            xviews::all_t<test_forward_range<int>>>;
    static_assert(xranges::input_range<ty2>);
    static_assert(!xranges::forward_range<ty2>);
    static_assert(!xranges::sized_range<ty2>);

    return true;
}

constexpr bool test03() {
    int u[] = {1, 2, 3, 4}, v[] = {4, 5, 6};
    auto z = xviews::zip_transform(
        std::plus{}, u | xviews::filter([](auto) { return true; }), v);
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

void test04() {
    extern int x[5];
    struct move_only {
        move_only() {}
        move_only(move_only&&) {}
        int operator()(int i, int j) const { return i + j; }
    };
    // P2494R2 Relaxing range adaptors to allow for move only types
    static_assert(requires { xviews::zip_transform(move_only{}, x, x); });
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    test04();
}
