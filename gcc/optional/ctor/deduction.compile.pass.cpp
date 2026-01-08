// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2017-2026 Free Software Foundation, Inc.
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

// { dg-do compile { target c++17 }  }

#include "rxx/optional.h"

#include <type_traits>

struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly&&);
    MoveOnly& operator=(MoveOnly&&);
};

int main() {
    __RXX optional x = 5;
    static_assert(std::is_same_v<decltype(x), __RXX optional<int>>);
    int y = 42;
    __RXX optional x2 = y;
    static_assert(std::is_same_v<decltype(x2), __RXX optional<int>>);
    int const z = 666;
    __RXX optional x3 = z;
    static_assert(std::is_same_v<decltype(x3), __RXX optional<int>>);
    __RXX optional mo = MoveOnly();
    static_assert(std::is_same_v<decltype(mo), __RXX optional<MoveOnly>>);
    mo = MoveOnly();

    __RXX optional copy = x;
    static_assert(std::is_same_v<decltype(copy), __RXX optional<int>>);
    __RXX optional move = std::move(mo);
    static_assert(std::is_same_v<decltype(move), __RXX optional<MoveOnly>>);
}
