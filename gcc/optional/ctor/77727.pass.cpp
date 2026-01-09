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

#include <cassert>

struct NonTransferable {
    int x;
    NonTransferable(int x) : x(x) {}
    NonTransferable(NonTransferable&&) = delete;
    NonTransferable& operator=(NonTransferable&&) = delete;
    operator int() { return x; }
};

int main() {
    __RXX optional<int> oi;
    __RXX optional<NonTransferable> ot(std::move(oi));
    assert(!ot);

    __RXX optional<int> oi2;
    __RXX optional<NonTransferable> ot2(oi2);
    assert(!ot);

    __RXX optional<int> oi3{42};
    __RXX optional<NonTransferable> ot3(std::move(oi3));
    assert(ot3 && *ot3 == 42);

    __RXX optional<int> oi4{666};
    __RXX optional<NonTransferable> ot4(oi4);
    assert(ot4 && *ot4 == 666);
}
