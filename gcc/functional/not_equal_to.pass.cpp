// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2019-2025 Free Software Foundation, Inc.
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

// { dg-do run { target c++20 } }

#include "rxx/functional/not_equal_to.h"

#include "../test_iterators.h"

#include <cassert>

namespace xranges = rxx::ranges;

// C++20 [range.cmp]

using F = xranges::not_equal_to;
static_assert(std::is_default_constructible_v<F>);
static_assert(std::is_copy_constructible_v<F>);
static_assert(std::is_move_constructible_v<F>);
static_assert(std::is_copy_assignable_v<F>);
static_assert(std::is_move_assignable_v<F>);

static_assert(!std::is_invocable_v<F>);
static_assert(!std::is_invocable_v<F, int&>);
static_assert(!std::is_invocable_v<F, int, void>);
static_assert(!std::is_invocable_v<F, int, void*>);
static_assert(std::is_nothrow_invocable_r_v<bool, F&, int&, int>);
static_assert(std::is_nothrow_invocable_r_v<bool, F, long const&, char>);
static_assert(std::is_nothrow_invocable_r_v<bool, F const&, short, int&>);
static_assert(std::is_nothrow_invocable_r_v<bool, F const, char const, char>);

using T = F::is_transparent; // required typedef

static_assert(!xranges::not_equal_to{}(99, 99.0));
static_assert(xranges::not_equal_to{}(99, 99.01));
static_assert(xranges::not_equal_to{}(99, 140L));

void test01() {
    F f;
    int a[2]{};
    assert(!f(&a, (void*)&a[0]));
    assert(f(&a, (void*)&a[1]));
    assert(!f(&a + 1, (void*)(a + 2)));
}

struct X {};
int operator==(X, X) noexcept {
    return 2;
}
int operator!=(X, X) {
    return 0;
}

static_assert(std::is_nothrow_invocable_r_v<bool, F&, X, X>);

void test02() {
    X x;
    F f;
    assert(!f(x, x));
}

struct A {
    bool operator==(A const&) const noexcept { return true; }
    bool operator==(A&&) const { return true; }
};

void test03() {
    A const a{};
    static_assert(noexcept(a == a));
    static_assert(!noexcept(a == A{}));
    static_assert(noexcept(xranges::not_equal_to{}(a, a)));
    static_assert(!noexcept(xranges::not_equal_to{}(a, A{})));
}

int main() {
    test01();
    test02();
    test03();
}
