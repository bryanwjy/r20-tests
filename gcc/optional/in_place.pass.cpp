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

int main() {
    // [20.5.5] In-place construction
    {
        __RXX optional<int> o{std::in_place};
        assert(o);
        assert(*o == int());

        static_assert(
            !std::is_convertible<std::in_place_t, __RXX optional<int>>(), "");
    }

    {
        __RXX optional<int> o{std::in_place, 42};
        assert(o);
        assert(*o == 42);
    }

    {
        __RXX optional<std::vector<int>> o{std::in_place, 18, 4};
        assert(o);
        assert(o->size() == 18);
        assert((*o)[17] == 4);
    }

    {
        __RXX optional<std::vector<int>> o{
            std::in_place, {18, 4}
};
        assert(o);
        assert(o->size() == 2);
        assert((*o)[0] == 18);
    }

    {
        __RXX optional<std::vector<int>> o{
            std::in_place, {18, 4},
             std::allocator<int>{}
};
        assert(o);
        assert(o->size() == 2);
        assert((*o)[0] == 18);
    }
}
