// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2026 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/optional.h"

#include <cassert>

struct aggressive_aggregate {
    int a;
    int b;
};

void test01() {
    __RXX optional<aggressive_aggregate> x;
    x.emplace(1, 2);
    assert(x->a == 1);
    assert(x->b == 2);
    x.emplace(1);
    assert(x->a == 1);
    assert(x->b == 0);
    x.emplace();
    assert(x->a == 0);
    assert(x->b == 0);
}

int main() {
    test01();
}
