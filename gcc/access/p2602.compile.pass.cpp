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

// P2602R2 Poison Pills are Too Toxic

#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct A {
    friend auto begin(A const&) -> int const*;
    friend auto end(A const&) -> int const*;
};

struct B {
    friend auto begin(B&) -> int*;
    friend auto end(B&) -> int*;
};

static_assert(xranges::range<A>);
static_assert(xranges::range<A const>);
static_assert(xranges::range<B>);
static_assert(!xranges::range<B const>);

class Test {
    friend size_t size(Test const&) { return 0; }
};

size_t f(Test t) {
    return xranges::size(t);
}
