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

#include <cassert>
#include <vector>

int counter = 0;

struct mixin_counter {
    mixin_counter() { ++counter; }
    mixin_counter(mixin_counter const&) { ++counter; }
    ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {};

int main() {
    using O = __RXX optional<value_type>;

    // Check __RXX nullopt_t and 'default' (= {}) assignment

    {
        O o;
        o = __RXX nullopt;
        assert(!o);
    }

    {
        O o{std::in_place};
        o = __RXX nullopt;
        assert(!o);
    }

    {
        O o;
        o = {};
        assert(!o);
    }

    {
        O o{std::in_place};
        o = {};
        assert(!o);
    }
    {
        __RXX optional<std::vector<int>> ovi{
            {1, 2, 3}
};
        assert(ovi->size() == 3);
        assert((*ovi)[0] == 1 && (*ovi)[1] == 2 && (*ovi)[2] == 3);
        ovi = {4, 5, 6, 7};
        assert(ovi->size() == 4);
        assert((*ovi)[0] == 4 && (*ovi)[1] == 5 && (*ovi)[2] == 6 &&
            (*ovi)[3] == 7);
    }
    assert(counter == 0);
}
