// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite
// Copyright (C) 2019-2026 Free Software Foundation, Inc.
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

#include "rxx/variant.h"

#include <cassert>

void test01() {
    struct Visitor {
        int operator()(int, void*) const { return 0; }
        int operator()(char& c, void* p) const { return &c == p; }
        int operator()(int i, char const* s) const { return s[i] == '\0'; }
        int operator()(char c, char const* s) const { return c == *s; }
    };

    __RXX variant<int, char> v1{'c'};
    __RXX variant<void*, char const*> v2{"chars"};

    auto res = __RXX visit<bool>(Visitor{}, v1, v2);
    static_assert(std::is_same_v<decltype(res), bool>);
    assert(res == true);

    static_assert(
        std::is_void_v<decltype(__RXX visit<void>(Visitor{}, v1, v2))>);
}

void test02() {
    struct Visitor {
        int operator()(double) { return 42; }
        double operator()(int) { return 0.02; }
    };
    __RXX variant<int, double> v;
    __RXX visit<int>(Visitor(), v);
    __RXX visit<void const>(Visitor(), v);
}

void test03() {
    // PR libstdc++/106589 - visit<void> rejects lambdas that do not return void
    auto visitor = [] { return 0; };
    __RXX visit<void>(visitor);
    __RXX visit<void>(static_cast<int (*)()>(visitor));
}

int main() {
    test01();
    test02();
    test03();
}
