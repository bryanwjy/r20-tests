// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2018-2026 Free Software Foundation, Inc.
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

// { dg-do run { target c++17 } }

#include "rxx/variant.h"

#include <cassert>

void test01() {
    __RXX variant<__RXX monostate> v1, v2;
    __RXX swap(v1, v2);
}

void test02() {
    __RXX variant<int> v1{1}, v2{2};
    __RXX swap(v1, v2);
    assert(__RXX get<0>(v1) == 2);
    assert(__RXX get<0>(v2) == 1);
}

void test03() {
    __RXX variant<double, int> v1{1}, v2{0.5};
    __RXX swap(v1, v2);
    assert(__RXX get<double>(v1) == 0.5);
    assert(__RXX get<int>(v2) == 1);
}

int main() {
    test01();
    test02();
    test03();
}
