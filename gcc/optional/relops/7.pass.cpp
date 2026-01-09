// { dg-do run { target c++17 }  }

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

#include "rxx/optional.h"

#include <cassert>
#include <string>

int main() {
    __RXX optional<int> o = 42;
    __RXX optional<int const> o2 = 666;
    assert(o == 42);
    assert(o != 43);
    assert(o < 43);
    assert(o > 41);
    assert(o <= 43);
    assert(o >= 41);
    assert(o2 == 666);
    assert(o2 != 667);
    assert(o2 < 667);
    assert(o2 > 665);
    assert(o2 <= 667);
    assert(o2 >= 665);
    assert(42 == o);
    assert(43 != o);
    assert(41 < o);
    assert(43 > o);
    assert(41 <= o);
    assert(43 >= o);
    assert(666 == o2);
    assert(667 != o2);
    assert(665 < o2);
    assert(667 > o2);
    assert(665 <= o2);
    assert(667 >= o2);
    __RXX optional<std::string> os = "jones";
    assert(os == "jones");
    assert(os != "bones");
    assert(os < "kones");
    assert(os > "hones");
    assert(os <= "kones");
    assert(os >= "hones");
    assert("jones" == os);
    assert("bones" != os);
    assert("hones" < os);
    assert("kones" > os);
    assert("hones" <= os);
    assert("kones" >= os);
    __RXX optional<int> oi = 42;
    __RXX optional<long int> ol = 666;
    assert(!(oi == ol));
    assert(!(ol == oi));
    assert(oi != ol);
    assert(ol != oi);
}
