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

static_assert(rxx::tests::is_customization_point_object(std::ranges::crbegin));

struct R1 {
    int i = 0;
    int j = 0;

    int const* begin() const;
    int const* end() const;

    int const* rbegin() const { return &i; }
    friend int const* rbegin(const R1&& r) { return &r.j; }
};

struct R1V // view on an R1
{
    R1& r;

    int const* begin() const;
    int const* end() const;

    friend long const* rbegin(R1V&) { return nullptr; }
    friend int const* rbegin(const R1V& rv) noexcept { return rv.r.rbegin(); }
};

// Allow ranges::end to work with R1V&&
template <>
constexpr bool std::ranges::enable_borrowed_range<R1V> = true;

void test01() {
    R1 r;
    const R1& c = r;
    assert(std::ranges::crbegin(r) == xranges::rbegin(c));
    assert(std::ranges::crbegin(c) == xranges::rbegin(c));

    R1V v{r};
    assert(std::ranges::crbegin(v) == xranges::rbegin(c));
    assert(std::ranges::crbegin(std::move(v)) == xranges::rbegin(c));

    const R1V cv{r};
    assert(std::ranges::crbegin(cv) == xranges::rbegin(c));
    assert(std::ranges::crbegin(std::move(cv)) == xranges::rbegin(c));
}

struct R2 {
    int a[2] = {};
    long l[2] = {};

    int const* begin() const { return a; }
    int const* end() const { return a + 2; }

    friend long const* begin(const R2&&); // not defined
    friend long const* end(const R2&&);   // not defined
};

// N.B. this is a lie, rbegin on an R2 rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<R2> = true;

void test02() {
    R2 r;
    const R2& c = r;
    assert(std::ranges::crbegin(r) == xranges::rbegin(c));
    assert(std::ranges::crbegin(c) == xranges::rbegin(c));

    assert(std::ranges::crbegin(std::move(r)) == xranges::rbegin(c));
    assert(std::ranges::crbegin(std::move(c)) == xranges::rbegin(c));
}

int main() {
    test01();
    test02();
}
