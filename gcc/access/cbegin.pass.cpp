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

static_assert(__RXX tests::is_customization_point_object(xranges::cbegin));

using std::same_as;

void test01() {
    int a[2] = {};

    static_assert(same_as<decltype(xranges::cbegin(a)), int const*>);
    static_assert(noexcept(xranges::cbegin(a)));
    assert(xranges::cbegin(a) == (a + 0));

    constexpr long b[2] = {};
    static_assert(xranges::cbegin(b) == (b + 0));
}

struct R {
    int a[4] = {0, 1, 2, 3};

    friend int* begin(R& r) { return r.a + 0; }
    friend int* begin(R&&); // this function is not defined
    friend int const* begin(R const& r) noexcept { return r.a + 2; }
    friend int const* begin(R const&&);       // this function is not defined
    friend int const* end(R const&) noexcept; // C++23 requires this.
};

struct RV // view on an R
{
    R& r;

    friend int* begin(RV&); // this function is not defined
    friend int const* begin(const RV& rv) noexcept {
        return begin(std::as_const(rv.r));
    }

    friend int const* end(const RV&) noexcept; // C++23 requires this.
};

// Allow ranges::begin to work with RV&&
template <>
constexpr bool std::ranges::enable_borrowed_range<RV> = true;

void test03() {
    R r;
    R const& c = r;
    assert(xranges::cbegin(r) == xranges::begin(c));
    assert(xranges::cbegin(c) == xranges::begin(c));

    RV v{r};
    assert(xranges::cbegin(std::move(v)) == xranges::begin(c));
    const RV cv{r};
    assert(xranges::cbegin(std::move(cv)) == xranges::begin(c));
}

struct RR {
    short s = 0;
    long l = 0;
    int a[4] = {0, 1, 2, 3};

    short* begin() noexcept { return &s; }
    long const* begin() const { return &l; }

    friend int* begin(RR& r) { return r.a + 0; }
    friend int* begin(RR&& r) { return r.a + 1; }
    friend int const* begin(const RR& r) { return r.a + 2; }
    friend int const* begin(const RR&& r) noexcept { return r.a + 3; }

    short* end() noexcept { return &s + 1; }   // C++23 requires this.
    long const* end() const { return &l + 1; } // C++23 requires this.
};

// N.B. this is a lie, cbegin on an RR rvalue will return a dangling pointer.
template <>
constexpr bool std::ranges::enable_borrowed_range<RR> = true;

void test04() {
    RR r;
    const RR& c = r;
    assert(xranges::cbegin(r) == xranges::begin(c));
    assert(xranges::cbegin(std::move(r)) == xranges::begin(c));
    assert(xranges::cbegin(c) == xranges::begin(c));
    assert(xranges::cbegin(std::move(c)) == xranges::begin(c));
}

void test05() {
    // LWG 4027 - possibly-const-range should prefer returning const R&
    auto r =
        std::views::single(0) | std::views::transform([](int) { return 0; });
    using C1 = decltype(xranges::cbegin(r));
    using C1 = decltype(std::cbegin(r));

    [](auto x) {
        auto r = std::views::single(x) | std::views::lazy_split(0);
        static_assert(!requires { (*xranges::cbegin(r)).front() = 42; });
        static_assert(!requires { (*std::cbegin(r)).front() = 42; });
    }(0);
}

int main() {
    test01();
    test03();
    test04();
    test05();
}
