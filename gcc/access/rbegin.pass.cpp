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

static_assert(rxx::tests::is_customization_point_object(xranges::rbegin));

struct R1 {
    int i = 0;
    int j = 0;

    constexpr int const* rbegin() const { return &i; }
    friend constexpr int const* rbegin(const R1&& r) { return &r.j; }
};

// N.B. this is a lie, rbegin on an R1 rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<R1> = true;

void test01() {
    constexpr R1 r;
    // decay-copy(t.rbegin()) if it is a valid expression
    // and its type I models input_or_output_iterator.
    static_assert(xranges::rbegin(r) == &r.i);
    static_assert(xranges::rbegin(std::move(r)) == &r.i);
}

struct R2 {
    int a[2] = {};

    constexpr int const* begin() const { return a; }
    constexpr int const* end() const { return a + 2; }

    friend constexpr long const* begin(const R2&&); // not defined
    friend constexpr long const* end(const R2&&);   // not defined
};

// N.B. this is a lie, begin/end on an R2 rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<R2> = true;

void test02() {
    constexpr R2 r;
    // Otherwise, decay-copy(rbegin(t)) if it is a valid expression
    // and its type I models input_or_output_iterator [...]
    static_assert(
        xranges::rbegin(r) == std::make_reverse_iterator(xranges::end(r)));
    static_assert(xranges::rbegin(std::move(r)) ==
        std::make_reverse_iterator(xranges::end(std::move(r))));
}

void test03() {
    struct R3 :
        rxx::tests::test_range<int,
            rxx::tests::bidirectional_iterator_wrapper> {
        R3(int (&a)[2]) : test_range(a) {}

        using test_range::begin;

        // Replace test_range::end() to return same type as begin()
        // so ranges::rbegin will wrap it in a reverse_iterator .
        auto end() & {
            using rxx::tests::bidirectional_iterator_wrapper;
            return bidirectional_iterator_wrapper<int>(bounds.last, &bounds);
        }
    };

    int a[2] = {};
    R3 r(a);

    // Otherwise, make_reverse_iterator(ranges::end(t)) if both ranges::begin(t)
    // and ranges::end(t) are valid expressions of the same type I which models
    // bidirectional_iterator.

    assert(xranges::rbegin(r) == std::make_reverse_iterator(xranges::end(r)));
}

int main() {
    test01();
    test02();
    test03();
}
