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

#include "rxx/ranges/join_view.h"

#include "rxx/algorithm.h"
#include "rxx/functional.h"
#include "rxx/ranges/basic_istream_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/lazy_split_view.h"
#include "rxx/ranges/ref_view.h"
#include "rxx/ranges/transform_view.h"

#include <cassert>
#include <sstream>
#include <string_view>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

void test01() {
    using namespace std::literals;
    std::string_view cs[] = {"the", "quick", "brown", "fox"};
    auto v = cs | xviews::join;
    assert(xranges::equal(v, "thequickbrownfox"sv));
    using R = decltype(v);
    static_assert(xranges::bidirectional_range<R>);
    static_assert(xranges::bidirectional_range<R const>);
    static_assert(xranges::common_range<R>);
    static_assert(xranges::common_range<R const>);
}

void test02() {
    auto v = (xviews::iota(0, 4) | xviews::transform([](int i) {
        return xviews::iota(0, i);
    }) | xviews::join);
    assert(xranges::equal(v, (int[]){0, 0, 1, 0, 1, 2}));
    using R = decltype(v);
    static_assert(xranges::input_range<R>);
    static_assert(!xranges::range<R const>);
    static_assert(!xranges::forward_range<R>);
    static_assert(!xranges::common_range<R const>);
}

void test03() {
    auto v = (xviews::iota(0, 4) |
        xviews::transform([](int i) { return xviews::iota(0, i); }) |
        xviews::filter([](auto) { return true; }) | xviews::join);
    assert(xranges::equal(v, (int[]){0, 0, 1, 0, 1, 2}));
    using R = decltype(v);
    static_assert(xranges::input_range<R>);
    static_assert(!xranges::range<R const>);
    static_assert(!xranges::forward_range<R>);
    static_assert(!xranges::common_range<R const>);
}

void test04() {
    auto v = (xviews::iota(0, 4) |
        xviews::transform([](int i) { return xviews::iota(0, i); }));
    auto v2 = xranges::ref_view{v};
    assert(xranges::equal(v2 | xviews::join, (int[]){0, 0, 1, 0, 1, 2}));
    using R = decltype(v2);
    static_assert(xranges::random_access_range<R>);
    static_assert(xranges::range<R const>);
    static_assert(xranges::common_range<R const>);
    static_assert(xranges::random_access_range<xranges::range_reference_t<R>>);
    static_assert(!std::is_reference_v<xranges::range_reference_t<R>>);
}

void test05() {
    using namespace std::literals;
    std::vector<std::string> x = {
        "the", " ", "quick", " ", "brown", " ", "fox"};
    auto v = x | xviews::join | xviews::lazy_split(' ');

    auto i = v.begin();
    assert(xranges::equal(*i++, "the"sv));
    assert(xranges::equal(*i++, "quick"sv));
    assert(xranges::equal(*i++, "brown"sv));
    assert(xranges::equal(*i++, "fox"sv));
    assert(i == v.end());
}

void test06() {
    std::vector<std::string> x = {""};
    auto i = std::counted_iterator(x.begin(), 1);
    auto r = xranges::subrange{i, std::default_sentinel};
    auto v = r | xviews::transform(std::identity{}) | xviews::join;

    // Verify that _Iterator<false> is implicitly convertible to
    // _Iterator<true>.
    static_assert(!std::same_as<decltype(xranges::begin(v)),
                  decltype(xranges::cbegin(v))>);
    auto a = std::cbegin(v);
    a = xranges::begin(v);

    // Verify that _Sentinel<false> is implicitly convertible to
    // _Sentinel<true>.
    static_assert(!xranges::common_range<decltype(v)>);
    static_assert(
        !std::same_as<decltype(xranges::end(v)), decltype(xranges::cend(v))>);
    auto b = xranges::cend(v);
    b = xranges::end(v);
}

void test07() {
    // LWG 3474. Nesting join_views is broken because of CTAD
    std::vector<std::vector<std::vector<int>>> nested_vectors = {
        {{1, 2, 3}, {4, 5}, {6}},
        {{7}, {8, 9}, {10, 11, 12}},
        {{13}}
    };
    auto joined = nested_vectors | xviews::join | xviews::join;

    using V = decltype(joined);
    static_assert(std::same_as<xranges::range_value_t<V>, int>);
}

void test08() {
    // LWG 3500. join_view::iterator::operator->() is bogus
    struct X {
        int a;
        constexpr X(int a) noexcept : a{a} {}
    };
    xranges::single_view<xranges::single_view<X>> s{
        std::in_place, std::in_place, 5};
    auto v = s | xviews::join;
    auto i = v.begin();
    assert(i->a == 5);
}

template <auto join = xviews::join>
void test09() {
    // Verify SFINAE behavior.
    static_assert(!requires { join(); });
    static_assert(!requires { join(0, 0); });
    static_assert(!requires { join(0); });
    static_assert(!requires { 0 | join; });
}

void test10() {
    // PR libstdc++/100290
    auto v = xviews::single(0) |
        xviews::transform([](auto const& s) { return xviews::single(s); }) |
        xviews::join;
    assert(xranges::next(v.begin()) == v.end());
}

void test11() {
    // Verify P2328 changes.
    int r[] = {1, 2, 3};
    auto v = r | xviews::transform([](int n) {
        return std::vector{
            {n, -n}
        };
    }) | xviews::join;
    assert(xranges::equal(v, (int[]){1, -1, 2, -2, 3, -3}));

    struct S {
        S() = default;
        S(S const&) = delete;
        S(S&&) = delete;
    };
    auto w = r | xviews::transform([](int) { return std::array<S, 2>{}; }) |
        xviews::join;
    for (auto& i : w)
        ;
}

void test12() {
    // PR libstdc++/101263
    constexpr auto b = [] {
        auto r = xviews::iota(0, 5) | xviews::lazy_split(0) | xviews::join;
        return r.begin() != r.end();
    }();
}

void test13() {
    // PR libstdc++/106320
    auto l = xviews::transform([](auto x) {
        return x | xviews::transform([i = 0](auto y) { return y; });
    });
    std::vector<std::vector<int>> v{
        {5, 6, 7}
    };
    (void)(v | l | xviews::join);
}

void test14() {
    // LWG 3569: join_view fails to support ranges of ranges with
    // non-default_initializable iterators
    auto ss = std::istringstream{"1 2 3"};
    auto v = xviews::single(xviews::istream<int>(ss));
    using inner = xranges::range_reference_t<decltype(v)>;
    static_assert(xranges::input_range<inner> &&
        !xranges::forward_range<inner> &&
        !std::default_initializable<xranges::iterator_t<inner>>);
    assert(xranges::equal(v | xviews::join, (int[]){1, 2, 3}));
}

int main() {
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
}
