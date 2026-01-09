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

static_assert(__RXX tests::is_customization_point_object(xranges::cdata));

template <typename T>
concept has_cdata = requires(T&& t) { xranges::cdata(std::forward<T>(t)); };

void test01() {
    struct R {
        int i = 0;
        int j = 0;

        // These overloads mean that range<R> and range<const R> are satisfied.
        int const* begin() const { throw; }
        int const* end() const { throw; }

        int* data() { return &j; }
        R const* data() const noexcept { return nullptr; }
    };
    static_assert(has_cdata<R&>);
    static_assert(has_cdata<R const&>);
    R r;
    assert(xranges::cdata(r) == (R*)nullptr);
    static_assert(noexcept(xranges::cdata(r)));
    R const& c = r;
    assert(xranges::cdata(c) == (R*)nullptr);
    static_assert(noexcept(xranges::cdata(c)));

    // not lvalues and not borrowed ranges
    static_assert(!has_cdata<R>);
    static_assert(!has_cdata<R const>);

    struct R2 {
        // These overloads mean that range<R2> and range<const R2> are
        // satisfied.
        int const* begin() const { throw; }
        int const* end() const { throw; }

        int i = 0;
        int j = 0;
        int* data() { return &j; }
        const R2* data() const noexcept { return nullptr; }
    };
    static_assert(has_cdata<R2&>);
    static_assert(has_cdata<const R2&>);
    R2 r2;
    assert(xranges::cdata(r2) == (R2*)nullptr);
    static_assert(noexcept(xranges::cdata(r2)));
    const R2& c2 = r2;
    assert(xranges::cdata(c2) == (R2*)nullptr);
    static_assert(noexcept(xranges::cdata(c2)));
}

void test02() {
    int a[] = {0, 1};
    assert(xranges::cdata(a) == a + 0);

    static_assert(has_cdata<int (&)[2]>);
    static_assert(!has_cdata<int (&&)[2]>);
}

struct R3 {
    static inline int i = 0;
    static inline long l = 0;

    int* data() &;              // this function is not defined
    friend long* begin(R3&& r); // not defined
    friend long const* begin(const R3& r) { return &r.l; }
    friend short const* begin(const R3&&); // not defined

    // C++23 needs these so that range<R3> is satisfied and so that
    // possibly-const-range<R3> is not the same type as R3.
    friend long* begin(R3&);
    friend long* end(R3&);
    friend long const* end(const R3& r);
};

template <>
constexpr bool std::ranges::enable_borrowed_range<R3> = true;

void test03() {
    static_assert(has_cdata<R3&>);
    static_assert(has_cdata<R3>); // borrowed range
    static_assert(has_cdata<const R3&>);
    static_assert(has_cdata<const R3>); // borrowed range

    R3 r;
    const R3& c = r;
    assert(xranges::cdata(r) == xranges::data(c));
    assert(xranges::cdata(std::move(r)) == xranges::data(c));
    assert(xranges::cdata(std::move(c)) == xranges::begin(c));
}

int main() {
    test01();
    test02();
    test03();
}
