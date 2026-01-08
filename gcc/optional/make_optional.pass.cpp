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
#include <tuple>
#include <vector>

struct combined {
    std::vector<int> v;
    std::tuple<int, int> t;
    template <class... Args>
    combined(std::initializer_list<int> il, Args&&... args)
        : v(il)
        , t(std::forward<Args>(args)...) {}
};

int main() {
    int const i = 42;
    auto o = __RXX make_optional(i);
    static_assert(std::is_same<decltype(o), __RXX optional<int>>(), "");
    assert(o && *o == 42);
    assert(&*o != &i);
    auto o2 = __RXX make_optional<std::tuple<int, int>>(1, 2);
    static_assert(
        std::is_same<decltype(o2), __RXX optional<std::tuple<int, int>>>(),
        "");
    assert(o2 && std::get<0>(*o2) == 1 && std::get<1>(*o2) == 2);
    auto o3 = __RXX make_optional<std::vector<int>>({42, 666});
    static_assert(
        std::is_same<decltype(o3), __RXX optional<std::vector<int>>>(), "");
    assert(o3 && (*o3)[0] == 42 && (*o3)[1] == 666);
    auto o4 = __RXX make_optional<combined>({42, 666});
    static_assert(std::is_same<decltype(o4), __RXX optional<combined>>(), "");
    assert(o4 && (o4->v)[0] == 42 && (o4->v)[1] == 666 &&
        std::get<0>(o4->t) == 0 && std::get<1>(o4->t) == 0);
    auto o5 = __RXX make_optional<combined>({1, 2}, 3, 4);
    static_assert(std::is_same<decltype(o5), __RXX optional<combined>>(), "");
    assert(o4 && (o5->v)[0] == 1 && (o5->v)[1] == 2 &&
        std::get<0>(o5->t) == 3 && std::get<1>(o5->t) == 4);
}
