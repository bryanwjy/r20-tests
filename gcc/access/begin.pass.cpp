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

using std::same_as;

static_assert(rxx::tests::is_customization_point_object(xranges::begin));

void test01() {
    int a[2] = {};

    static_assert(same_as<decltype(xranges::begin(a)), decltype(a + 0)>);
    static_assert(noexcept(xranges::begin(a)));
    assert(xranges::begin(a) == (a + 0));

    constexpr long b[2] = {};
    static_assert(xranges::begin(b) == (b + 0));

    struct X {};
    using A = X[]; // unbounded array
    extern A& f();
    static_assert(same_as<decltype(xranges::begin(f())), X*>);
}

void test02() {
    using rxx::tests::input_iterator_wrapper;
    using rxx::tests::output_iterator_wrapper;
    using rxx::tests::random_access_iterator_wrapper;
    using rxx::tests::test_range;

    int a[] = {0, 1};

    test_range<int, random_access_iterator_wrapper> r(a);
    static_assert(same_as<decltype(xranges::begin(r)), decltype(r.begin())>);
    assert(xranges::begin(r) == r.begin());

    test_range<int, input_iterator_wrapper> i(a);
    static_assert(same_as<decltype(xranges::begin(i)), decltype(i.begin())>);
    assert(xranges::begin(i) == i.begin());

    test_range<int, output_iterator_wrapper> o(a);
    static_assert(same_as<decltype(xranges::begin(o)), decltype(o.begin())>);
    *xranges::begin(o) = 99;
    assert(a[0] == 99);
}

struct R {
    int a[4] = {0, 1, 2, 3};

    friend int* begin(R& r) { return r.a + 0; }
    friend int* begin(R&& r); // this overload is not defined
    friend int const* begin(R const& r) noexcept { return r.a + 2; }
    friend int const* begin(R const&& r) noexcept; // not defined
};

struct RV // view on an R
{
    R& r;

    friend int* begin(RV& rv) { return begin(rv.r); }
    friend int const* begin(const RV& rv) noexcept { return begin(rv.r); }
};

// Allow ranges::begin to work with RV&&
template <>
constexpr bool std::ranges::enable_borrowed_range<RV> = true;

void test03() {
    R r;
    R const& c = r;

    static_assert(same_as<decltype(xranges::begin(r)), decltype(begin(r))>);
    static_assert(!noexcept(xranges::begin(r)));
    assert(xranges::begin(r) == begin(r));

    static_assert(same_as<decltype(xranges::begin(c)), decltype(begin(c))>);
    static_assert(noexcept(xranges::begin(c)));
    assert(xranges::begin(c) == begin(c));

    RV v{r};
    // enable_borrowed_range<RV> allows ranges::begin to work for rvalues,
    // but it will call v.begin() or begin(v) on an lvalue:
    static_assert(
        same_as<decltype(xranges::begin(std::move(v))), decltype(begin(v))>);
    static_assert(!noexcept(xranges::begin(std::move(v))));
    assert(xranges::begin(std::move(v)) == begin(v));

    const RV cv{r};
    static_assert(
        same_as<decltype(xranges::begin(std::move(cv))), decltype(begin(cv))>);
    static_assert(noexcept(xranges::begin(std::move(cv))));
    assert(xranges::begin(std::move(cv)) == begin(cv));
}

struct RR {
    short s = 0;
    long l = 0;
    int a[4] = {0, 1, 2, 3};

    short* begin() noexcept { return &s; }
    long const* begin() const { return &l; }

    friend int* begin(RR& r) noexcept { return r.a + 0; }
    friend int* begin(RR&& r); // not defined
    friend int const* begin(const RR& r) { return r.a + 2; }
    friend int const* begin(const RR&& r) noexcept; // not defined
};

// N.B. this is a lie, begin on an RR rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<RR> = true;

void test04() {
    RR r;
    const RR& c = r;
    assert(xranges::begin(r) == &r.s);
    static_assert(noexcept(xranges::begin(r)));

    // calls r.begin() on an lvalue, not rvalue
    assert(xranges::begin(std::move(r)) == xranges::begin(r));
    static_assert(noexcept(xranges::begin(std::move(r))));

    assert(xranges::begin(c) == &r.l);
    static_assert(!noexcept(xranges::begin(c)));

    // calls r.begin() on a const lvalue, not rvalue
    assert(xranges::begin(std::move(c)) == xranges::begin(c));
    static_assert(!noexcept(xranges::begin(std::move(c))));
}

int main() {
    test01();
    test02();
    test03();
    test04();
}
