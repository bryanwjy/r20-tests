// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2013-2026 Free Software Foundation, Inc.
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

#include <memory>
#include <string>

int main() {
    {
        struct X {
            explicit X(int) {}
        };
        __RXX optional<X> ox{42};
        __RXX optional<X> ox2 = 42; // { dg-error "conversion" }
        __RXX optional<std::unique_ptr<int>> oup{new int};
        __RXX optional<std::unique_ptr<int>> oup2 =
            new int; // { dg-error "conversion" }
        struct U {
            explicit U(std::in_place_t);
        };
        __RXX optional<U> ou(std::in_place); // { dg-error "no matching" }
    }
}
// { dg-prune-output "no type .*enable_if" }
// { dg-prune-output "no matching function for call to 'main..::U::U..'" }
