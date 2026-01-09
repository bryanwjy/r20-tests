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

// { dg-do run { target c++17 } }

#include "rxx/variant.h"

#include <cassert>

void test01() {
    struct ThrowingConversion {
        operator int() { throw 0; }
    };

    __RXX variant<float, char, int> v{'a'};
    try {
        ThrowingConversion x;
        v.emplace<2>(x);
        assert(false);
    } catch (int) {
        assert(!v.valueless_by_exception());
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == 'a');
    }
}

void test02() {
    struct ThrowingConstructor {
        ThrowingConstructor(std::initializer_list<int>, char) { throw 1; }
    };

    __RXX variant<float, char, ThrowingConstructor> v{'a'};
    try {
        v.emplace<2>({1, 2, 3}, '4');
        assert(false);
    } catch (int) {
        assert(!v.valueless_by_exception());
        assert(v.index() == 1);
        assert(__RXX get<1>(v) == 'a');
    }
}

int main() {
    test01();
    test02();
}
