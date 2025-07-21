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

#include "rxx/ranges/join_with_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/empty_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/split_view.h"
#include "rxx/ranges/transform_view.h"

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::ranges::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
using namespace std::literals;

constexpr bool test01() {
    std::string_view rs[] = {"hello", "world"};
    auto v = rs | xviews::join_with(' ');
    assert(xranges::equal(v | xviews::split(' '), rs, xranges::equal));
    auto i = v.begin(), j = v.begin();
    assert(i == j);
    ++i;
    i++;
    assert(i != j);
    assert(*i == 'l');
    --i;
    i--;
    assert(*i == 'h');
    return true;
}

constexpr bool test02() {
    std::string_view rs[] = {"the", "quick", "brown", "fox"};
    auto v = rs | xviews::transform([](auto x) { return x; }) |
        xviews::filter([](auto) { return true; });
    assert(xranges::equal(
        v | xviews::join_with(xviews::empty<char>), "thequickbrownfox"sv));
    assert(xranges::equal(v | xviews::join_with('-'), "the-quick-brown-fox"sv));
    assert(xranges::equal(
        v | xviews::join_with("--"sv), "the--quick--brown--fox"sv));
    assert(xranges::empty(xviews::empty<int[3]> | xviews::join_with(0)));
    assert(xranges::equal(
        xviews::single(std::array{42}) | xviews::join_with(0), (int[]){42}));
    return true;
}

constexpr bool test03() {
    using rxx::tests::test_bidirectional_range;
    using rxx::tests::test_forward_range;
    using rxx::tests::test_input_range;

    using ty1 = xranges::join_with_view<
        xviews::all_t<test_input_range<test_input_range<int>>>,
        xviews::all_t<test_forward_range<int>>>;
    static_assert(xranges::input_range<ty1>);
    static_assert(!xranges::forward_range<ty1>);
    static_assert(!xranges::common_range<ty1>);

    using ty2 = xranges::join_with_view<
        xviews::all_t<test_forward_range<test_forward_range<int>>>,
        xviews::all_t<test_forward_range<int>>>;
    static_assert(xranges::forward_range<ty2>);
    static_assert(!xranges::bidirectional_range<ty2>);
    static_assert(!xranges::common_range<ty2>);

    using ty3 =
        xranges::join_with_view<xviews::all_t<std::array<std::string_view, 3>>,
            std::string_view>;
    static_assert(xranges::bidirectional_range<ty3>);
    static_assert(!xranges::random_access_range<ty3>);
    static_assert(xranges::common_range<ty3>);

    return true;
}

constexpr bool test04() {
    std::string rs[] = {"a", "", "b", "", "c"};
    auto v = rs | xviews::join_with(' ');
    assert(xranges::equal(v, "a  b  c"sv));
    auto i = v.begin();
    auto j = xranges::next(i, 3);
    xranges::iter_swap(i, j);
    *j = xranges::iter_move(i);
    assert(xranges::equal(v, "b  b  c"sv));
    return true;
}

int main() {
    static_assert(test01());
    static_assert(test02());
    static_assert(test03());
    static_assert(test04());
}
