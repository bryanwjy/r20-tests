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

namespace xranges = __RXX ranges;

#include <utility> // as_const

static_assert(__RXX tests::is_customization_point_object(xranges::cend));

using std::same_as;

void test01() {
    int a[2] = {};

    static_assert(same_as<decltype(xranges::cend(a)), int const*>);
    static_assert(noexcept(xranges::cend(a)));
    assert(xranges::cend(a) == (a + 2));
}

struct R {
    int a[4] = {0, 1, 2, 3};

    int const* begin() const { return nullptr; }
    friend int const* begin(R const&&) noexcept { return nullptr; }

    // Should be ignored because it doesn't return a sentinel for int*
    long const* end() const { return nullptr; }

    friend int* end(R& r) { return r.a + 0; }
    friend int* end(R&& r) { return r.a + 1; }
    friend int const* end(R const& r) noexcept { return r.a + 2; }
    friend int const* end(R const&& r) noexcept { return r.a + 3; }
};

struct RV // view on an R
{
    R& r;

    friend int const* begin(RV& rv) { return rv.r.begin(); }
    friend int* end(RV& rv) { return end(rv.r); }
    friend int const* begin(const RV& rv) noexcept { return rv.r.begin(); }
    friend int const* end(const RV& rv) noexcept {
        return end(std::as_const(rv.r));
    }
};

// Allow ranges::end to work with RV&&
template <>
constexpr bool std::ranges::enable_borrowed_range<RV> = true;

void test03() {
    R r;
    R const& c = r;
    assert(xranges::cend(r) == xranges::end(c));
    assert(xranges::cend(c) == xranges::end(c));

    RV v{r};
    assert(xranges::cend(std::move(v)) == xranges::end(c));

    const RV cv{r};
    assert(xranges::cend(std::move(cv)) == xranges::end(c));
}

struct RR {
    short s = 0;
    short l = 0;
    int a[4] = {0, 1, 2, 3};

    void const* begin() const; // return type not an iterator

    friend int* end(RR&) { throw 1; }
    short* end() noexcept { return &s; }

    friend short const* begin(const RR&) noexcept;
    short const* end() const { return &l; }

    friend int* begin(RR&&) noexcept;
    friend int* end(RR&& r) { return r.a + 1; }

    friend int const* begin(const RR&&) noexcept;
    friend int const* end(const RR&& r) noexcept { return r.a + 3; }
};

// N.B. this is a lie, begin/end on an RR rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<RR> = true;

void test04() {
    RR r;
    const RR& c = r;
    assert(xranges::cend(r) == xranges::end(c));
    assert(xranges::cend(c) == xranges::end(c));

    assert(xranges::cend(std::move(r)) == xranges::end(c));
    assert(xranges::cend(std::move(c)) == xranges::end(c));
}

int main() {
    test01();
    test03();
    test04();
}
