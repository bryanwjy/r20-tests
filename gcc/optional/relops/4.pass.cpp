// { dg-do run { target c++17 }  }

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
#include <string>
#include <tuple>

namespace ns {
struct value_type {
    int i;
    std::string s;
};

bool operator==(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator!=(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

bool operator>(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s);
}

bool operator<=(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s);
}

bool operator>=(value_type const& lhs, value_type const& rhs) {
    return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s);
}

} // namespace ns

int main() {
    using ns::value_type;
    using O = __RXX optional<value_type>;

    value_type const reference{42, "forty-two"};

    {
        O o;
        assert(o < reference);
        assert(!(reference < o));
        assert(!(o > reference));
        assert(reference > o);
        assert(o <= reference);
        assert(!(reference <= o));
        assert(!(o >= reference));
        assert(reference >= o);
    }

    {
        O o{
            value_type{11, "eleventy"}
        };
        assert(o < reference);
        assert(!(reference < o));
        assert(!(o > reference));
        assert(reference > o);
        assert(o <= reference);
        assert(!(reference <= o));
        assert(!(o >= reference));
        assert(reference >= o);
    }

    {
        O o{
            value_type{42, "forty-two"}
        };
        assert(!(o < reference));
        assert(!(reference < o));
        assert(!(o > reference));
        assert(!(reference > o));
        assert(o <= reference);
        assert(reference <= o);
        assert(o >= reference);
        assert(reference >= o);
    }
}
