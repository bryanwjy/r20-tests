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

#include "rxx/ranges/lazy_split_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"
#include "rxx/ranges/common_view.h"
#include "rxx/ranges/filter_view.h"
#include "rxx/ranges/join_view.h"
#include "rxx/ranges/ref_view.h"
#include "rxx/ranges/transform_view.h"

#include <cassert>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using __RXX tests::forward_iterator_wrapper;
using __RXX tests::input_iterator_wrapper;
using __RXX tests::test_range;

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

using namespace std::literals;

void test01() {
    auto x = "the  quick  brown  fox"sv;
    auto p = std::string{"  "};
    auto v = x |
        xviews::lazy_split(
            xviews::all(p)); // xviews::all is needed here after P2281.
    auto i = v.begin();
    assert(xranges::equal(*i++, "the"sv));
    assert(xranges::equal(*i++, "quick"sv));
    assert(xranges::equal(*i++, "brown"sv));
    assert(xranges::equal(*i++, "fox"sv));
    assert(i == v.end());
}

void test02() {
    auto x = "the quick brown fox"sv;
    auto v = x | xviews::lazy_split(' ');
    auto i = v.begin();
    assert(xranges::equal(*i++, "the"sv));
    assert(xranges::equal(*i++, "quick"sv));
    assert(xranges::equal(*i++, "brown"sv));
    assert(xranges::equal(*i++, "fox"sv));
    assert(i == v.end());
}

void test03() {
    char x[] = "the quick brown fox";
    test_range<char, forward_iterator_wrapper> rx(x, x + sizeof(x) - 1);
    auto v = rx | xviews::lazy_split(' ');
    auto i = v.begin();
    assert(xranges::equal(*i++, "the"sv));
    assert(xranges::equal(*i++, "quick"sv));
    assert(xranges::equal(*i++, "brown"sv));
    assert(xranges::equal(*i++, "fox"sv));
    assert(i == v.end());
}

void test04() {
    auto x = "the  quick  brown  fox"sv;
    std::initializer_list<char> p = {' ', ' '};
    static_assert(!xranges::view<decltype(p)>);
    static_assert(std::same_as<decltype(p | xviews::all),
        xranges::ref_view<decltype(p)>>);
    auto v = x |
        xviews::lazy_split(
            xviews::all(p)); // xviews::all is needed here after P2281.
    auto i = v.begin();
    assert(xranges::equal(*i++, "the"sv));
    assert(xranges::equal(*i++, "quick"sv));
    assert(xranges::equal(*i++, "brown"sv));
    assert(xranges::equal(*i++, "fox"sv));
    assert(i == v.end());
}

void test05() {
    auto as_string = [](xranges::view auto rng) {
        auto in = rng | xviews::common;
        return std::string(in.begin(), in.end());
    };
    std::string str =
        "Now is the time for all good men to come to the aid of their county.";
    auto rng = str | xviews::lazy_split(' ') | xviews::transform(as_string) |
        xviews::common;
    std::vector<std::string> words(rng.begin(), rng.end());
    auto not_space_p = [](char c) { return c != ' '; };
    assert(xranges::equal(
        words | xviews::join, str | xviews::filter(not_space_p)));
}

void test06() {
    std::string str = "hello world";
    auto v = str | xviews::transform(std::identity{}) | xviews::lazy_split(' ');

    // Verify that _Iterator<false> is implicitly convertible to
    // _Iterator<true>.
    static_assert(!std::same_as<decltype(xranges::begin(v)),
        decltype(xranges::cbegin(v))>);
    auto b = xranges::cbegin(v);
    b = xranges::begin(v);
}

void test07() {
    char str[] = "banana split";
    auto split = str | xviews::lazy_split(' ');
    auto val = *split.begin();
    auto b = val.begin();
    auto b2 = b++;
    static_assert(noexcept(iter_move(b)));
    static_assert(noexcept(iter_swap(b, b2)));
}

void test08() {
    char x[] = "the quick brown fox";
    test_range<char, input_iterator_wrapper> rx(x, x + sizeof(x) - 1);
    auto v = rx | xviews::lazy_split(' ');
    auto i = v.begin();
    assert(xranges::equal(*i, "the"sv));
    ++i;
    assert(xranges::equal(*i, "quick"sv));
    ++i;
    assert(xranges::equal(*i, "brown"sv));
    ++i;
    assert(xranges::equal(*i, "fox"sv));
    ++i;
    assert(i == v.end());
}

template <auto lazy_split = xviews::lazy_split>
void test09() {
    // Verify SFINAE behavior.
    std::string s, p;
    static_assert(!requires { lazy_split(); });
    static_assert(!requires { lazy_split(s, p, 0); });
    static_assert(!requires { lazy_split(p)(); });
    static_assert(!requires { s | lazy_split; });

    // Test the case where the closure object is used as an rvalue and therefore
    // the copy of p is forwarded as an rvalue.
    // This used to be invalid, but is now well-formed after P2415R2 relaxed
    // the requirements of viewable_range to admit rvalue non-view non-borrowed
    // ranges such as std::string&&.
    static_assert(requires { s | lazy_split(p); });
    static_assert(requires { lazy_split(p)(s); });
    static_assert(requires { s | (lazy_split(p) | xviews::all); });
    static_assert(requires { (lazy_split(p) | xviews::all)(s); });

    static_assert(requires { s | lazy_split(xviews::all(p)); });
    static_assert(requires { lazy_split(xviews::all(p))(s); });
    static_assert(requires { s | (lazy_split(xviews::all(p)) | xviews::all); });
    static_assert(requires { (lazy_split(xviews::all(p)) | xviews::all)(s); });

    auto adapt = lazy_split(p);
    static_assert(requires { s | adapt; });
    static_assert(requires { adapt(s); });

    auto adapt2 = lazy_split(p) | xviews::all;
    static_assert(requires { s | adapt2; });
    static_assert(requires { adapt2(s); });
}

void test10() {
    // LWG 3505
    auto to_string = [](auto r) {
        return std::string(r.begin(), xranges::next(r.begin(), r.end()));
    };
    auto v =
        "xxyx"sv | xviews::lazy_split("xy"sv) | xviews::transform(to_string);
    assert(xranges::equal(v, (std::string_view[]){"x", "x"}));
}

void test11() {
    // LWG 3478
    auto v = xviews::lazy_split("text"sv, "text"sv);
    auto i = v.begin();
    assert(xranges::empty(*i++));
    assert(xranges::empty(*i++));
    assert(i == v.end());

    static_assert(xranges::distance(xviews::lazy_split(" text "sv, ' ')) == 3);
    static_assert(
        xranges::distance(xviews::lazy_split(" t e x t "sv, ' ')) == 6);
    static_assert(
        xranges::distance(xviews::lazy_split("  text  "sv, "  "sv)) == 3);
    static_assert(
        xranges::distance(xviews::lazy_split("  text    "sv, "  "sv)) == 4);
    static_assert(
        xranges::distance(xviews::lazy_split("  text     "sv, "  "sv)) == 4);
    static_assert(xranges::distance(xviews::lazy_split("t"sv, 't')) == 2);
    static_assert(xranges::distance(xviews::lazy_split("text"sv, ""sv)) == 4);
}

constexpr bool test12() {
    // LWG 3904
    auto r = xviews::single(0) | xviews::lazy_split(0);
    auto i = r.begin();
    ++i;
    assert(i != r.end());
    decltype(std::as_const(r).begin()) j = i;
    assert(j != r.end());

    return true;
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
    static_assert(test12());
}
