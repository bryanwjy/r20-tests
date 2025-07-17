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

#include <utility> // as_const

static_assert(rxx::tests::is_customization_point_object(xranges::end));

using std::same_as;

void test01() {
    int a[2] = {};

    // t + extent_v<T> if E is of array type T.

    static_assert(same_as<decltype(xranges::end(a)), decltype(a + 2)>);
    static_assert(noexcept(xranges::end(a)));
    assert(xranges::end(a) == (a + 2));
}

void test02() {
    using rxx::tests::input_iterator_wrapper;
    using rxx::tests::output_iterator_wrapper;
    using rxx::tests::random_access_iterator_wrapper;
    using rxx::tests::test_range;

    int a[] = {0, 1};

    // Otherwise, decay-copy(t.end()) if it is a valid expression
    // and its type S models sentinel_for<decltype(ranges::begin(E))>.

    test_range<int, random_access_iterator_wrapper> r(a);
    static_assert(same_as<decltype(xranges::end(r)), decltype(r.end())>);
    assert(xranges::end(r) == std::ranges::next(r.begin(), 2));

    test_range<int, input_iterator_wrapper> i(a);
    static_assert(same_as<decltype(xranges::end(i)), decltype(i.end())>);
    assert(xranges::end(i) == std::ranges::next(i.begin(), 2));

    test_range<int, output_iterator_wrapper> o(a);
    static_assert(same_as<decltype(xranges::end(o)), decltype(o.end())>);
    assert(xranges::end(o) == std::ranges::next(o.begin(), 2));
}

struct R {
    int a[4] = {0, 1, 2, 3};

    int const* begin() const;
    friend int* begin(R&&) noexcept;
    friend int const* begin(R const&&) noexcept;

    // Should be ignored because it doesn't return a sentinel for int*
    long const* end() const;

    friend int* end(R& r) { return r.a + 0; }
    friend int* end(R&& r) { return r.a + 1; }
    friend int const* end(R const& r) noexcept { return r.a + 2; }
    friend int const* end(R const&& r) noexcept { return r.a + 3; }
};

struct RV // view on an R
{
    R& r;

    int const* begin() const;

    friend int* end(RV& v) noexcept { return end(v.r); }
    friend int const* end(const RV& v) { return end(std::as_const(v.r)); }
};

// Allow ranges::begin to work with RV&&
template <>
constexpr bool std::ranges::enable_borrowed_range<RV> = true;

void test03() {
    R r;
    R const& c = r;

    // Otherwise, decay-copy(end(t)) if it is a valid expression
    // and its type S models sentinel_for<decltype(ranges::begin(E))>.

    static_assert(same_as<decltype(xranges::end(r)), decltype(end(r))>);
    static_assert(!noexcept(xranges::end(r)));
    assert(xranges::end(r) == end(r));

    static_assert(same_as<decltype(xranges::end(c)), decltype(end(c))>);
    static_assert(noexcept(xranges::end(c)));
    assert(xranges::end(c) == end(c));

    RV v{r};
    static_assert(
        same_as<decltype(xranges::end(std::move(v))), decltype(end(r))>);
    static_assert(noexcept(xranges::end(std::move(v))));
    assert(xranges::end(std::move(v)) == end(r));

    const RV cv{r};
    static_assert(
        same_as<decltype(xranges::end(std::move(cv))), decltype(end(c))>);
    static_assert(!noexcept(xranges::end(std::move(cv))));
    assert(xranges::end(std::move(cv)) == end(c));
}

struct RR {
    short s = 0;
    long l = 0;
    int a[4] = {0, 1, 2, 3};

    void const* begin() const; // return type not an iterator

    friend short const* begin(RR&) noexcept;
    short* end() noexcept { return &s; }

    friend long const* begin(const RR&) noexcept;
    long const* end() const { return &l; }

    friend int const* begin(RR&&) noexcept;
    friend int* end(RR&) { throw 1; } // not valid for rvalues
    friend int* end(RR&& r) { return r.a + 1; }

    friend int const* begin(const RR&&) noexcept;
    friend int const* end(const RR&) { throw 1; } // not valid for rvalues
    friend int const* end(const RR&& r) noexcept { return r.a + 3; }
};

// N.B. this is a lie, end on an RR rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<RR> = true;

void test04() {
    RR r;
    const RR& c = r;
    assert(xranges::end(r) == &r.s);
    static_assert(noexcept(xranges::end(r)));

    assert(xranges::end(std::move(r)) == &r.s);
    static_assert(noexcept(xranges::end(std::move(r))));

    assert(xranges::end(c) == &r.l);
    static_assert(!noexcept(xranges::end(c)));

    assert(xranges::end(std::move(c)) == &r.l);
    static_assert(!noexcept(xranges::end(std::move(c))));
}

int main() {
    test01();
    test02();
    test03();
    test04();
}
