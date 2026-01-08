// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2016-2026 Free Software Foundation, Inc.
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

#include "rxx/optional.h"

// Swappable.
struct A {};

static_assert(std::is_nothrow_swappable_v<A>);
static_assert(std::is_nothrow_swappable_v<__RXX optional<A>>);

// Swappable, but might throw.
struct B {};
void swap(B&, B&) noexcept(false);

static_assert(std::is_swappable_v<__RXX optional<B>>);
static_assert(!std::is_nothrow_swappable_v<__RXX optional<B>>);

// Not swappable, and optional<C> not swappable
struct C {
    C(C&&) = delete;
};
void swap(C&, C&) = delete;
static_assert(!std::is_swappable_v<__RXX optional<C>>);

// Not swappable, and optional<D> not swappable
struct D {
    D(D&&) = delete;
};

static_assert(!std::is_swappable_v<__RXX optional<D>>);

struct E {};
void swap(E&, E&) = delete;
static_assert(std::is_swappable_v<__RXX optional<E>>);

// Swappable, and optional<F> not swappable
struct F {
    F(F&&) = delete;
};
void swap(F&, F&);
static_assert(!std::is_swappable_v<__RXX optional<F>>);
