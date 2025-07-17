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

// This used to be:
// PR testsuite/101782
// attribute-specifier-seq cannot follow requires-clause with -fconcepts-ts
#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

static_assert(rxx::tests::is_customization_point_object(xranges::crend));

struct R1 {
    int i = 0;
    int j = 0;

    int const* begin() const;
    int const* end() const;

    constexpr int const* rbegin() const { return &i; }
    constexpr int const* rend() const { return &i + 1; }
    friend constexpr int const* rbegin(const R1&& r) { return &r.j; }
    friend constexpr int const* rend(const R1&& r) { return &r.j + 1; }
};

// N.B. this is a lie, rend on an R1 rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<R1> = true;

void test01() {
    R1 r;
    const R1& c = r;
    assert(xranges::crend(r) == xranges::rend(c));
    assert(xranges::crend(c) == xranges::rend(c));
    assert(xranges::crend(std::move(r)) == xranges::rend(c));
    assert(xranges::crend(std::move(c)) == xranges::rend(c));
}

struct R2 {
    int a[2] = {};
    long l[2] = {};

    int const* begin() const { return a; }
    int const* end() const { return a + 2; }

    friend long const* begin(const R2&& r) { return r.l; }
    friend long const* end(const R2&& r) { return r.l + 2; }
};

// N.B. this is a lie, rend on an R2 rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<R2> = true;

void test02() {
    R2 r;
    const R2& c = r;
    assert(xranges::crend(r) == xranges::rend(c));
    assert(xranges::crend(c) == xranges::rend(c));
    assert(xranges::crend(std::move(r)) == xranges::rend(std::move(c)));
    assert(xranges::crend(std::move(c)) == xranges::rend(std::move(c)));
}

struct R3 {
    int i = 0;

    int const* begin() const;
    int const* end() const;

    int const* rbegin() const noexcept { return &i + 1; }
    long const* rend() const noexcept {
        return nullptr;
    } // not a sentinel for rbegin()

    friend long const* rbegin(const R3&) noexcept { return nullptr; }
    friend int const* rend(const R3& r) { return &r.i; }
};

struct R4 {
    int i = 0;

    // These members mean that range<R4> and range<const R4> are satisfied.
    short const* begin() const { return 0; }
    short const* end() const { return 0; }

    int const* rbegin() const noexcept { return &i + 1; }
    long const* rend() const noexcept {
        return nullptr;
    } // not a sentinel for rbegin()

    friend long const* rbegin(const R4&) noexcept { return nullptr; }
    friend int const* rend(const R4& r) { return &r.i; }
};

void test03() {
    R3 r;
    const R3& c = r;
    assert(xranges::crend(r) == xranges::rend(c));
    static_assert(!noexcept(xranges::crend(r)));
    assert(xranges::crend(c) == xranges::rend(c));
    static_assert(!noexcept(xranges::crend(c)));

    R4 r4;
    const R4& c4 = r4;
    auto b = xranges::rbegin(r4);
    auto s0 = xranges::rend(r4);
    static_assert(xranges::details::unqualified_rend<R4&>);
    auto s = xranges::crend(r4);
    auto s2 = xranges::rend(c4);
    // assert( xranges::crend(r4) == xranges::rend(c4) );
}

void test04() {
    int a[2] = {};
    auto const& c = a;
    assert(xranges::crend(a) == xranges::rend(c));
    assert(xranges::crend(c) == xranges::rend(c));
}

int main() {
    test01();
    test02();
    test03();
    test04();
}
