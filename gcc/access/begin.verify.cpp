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

#include "rxx/ranges.h"

#include <cassert>

namespace xranges = __RXX ranges;

auto test01() {
    using A = int[2];
    extern A&& f(); // rvalue of type that doesn't satisfy borrowed_range
    return xranges::begin(f()); // { dg-error "no match" }
}

struct incomplete;
extern incomplete array[2];

auto test02() {
    return xranges::begin(array); // { dg-error "here" }
}
// { dg-error "incomplete type" "" { target *-*-* } 0 }
