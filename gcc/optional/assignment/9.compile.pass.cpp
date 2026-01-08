// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2018-2026 Free Software Foundation, Inc.
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

constexpr bool f() {
    __RXX optional<int> o1{42};
    __RXX optional<int> o2;
    o2 = o1;
    return (o1 == o2);
}

constexpr bool f2() {
    __RXX optional<int> o1{42};
    __RXX optional<int> o2;
    __RXX optional<int> o3;
    o2 = o1;
    o3 = std::move(o2);
    return (o1 == o3);
}

void g() {
    constexpr bool b = f();
    static_assert(b);
    constexpr bool b2 = f2();
    static_assert(b2);
}

struct NonTrivialButConstexpr {
    int dummy;
    NonTrivialButConstexpr() = default;
    constexpr NonTrivialButConstexpr(int val) : dummy(val) {}
    NonTrivialButConstexpr(NonTrivialButConstexpr const&) = default;
    NonTrivialButConstexpr(NonTrivialButConstexpr&&) = default;
    constexpr NonTrivialButConstexpr& operator=(
        NonTrivialButConstexpr const& other) {
        dummy = other.dummy;
        return *this;
    }
    constexpr NonTrivialButConstexpr& operator=(
        NonTrivialButConstexpr&& other) {
        dummy = other.dummy;
        return *this;
    }
};

constexpr bool f3() {
    __RXX optional<NonTrivialButConstexpr> d1, d2;
    d1 = d2;
    __RXX optional<NonTrivialButConstexpr> o1{42};
    __RXX optional<NonTrivialButConstexpr> o2{22};
    o2 = o1;
    return ((*o1).dummy == (*o2).dummy && o1->dummy == o2->dummy);
}

constexpr bool f4() {
    __RXX optional<NonTrivialButConstexpr> d1, d2;
    d1 = std::move(d2);
    __RXX optional<NonTrivialButConstexpr> o1{42};
    __RXX optional<NonTrivialButConstexpr> o2{22};
    __RXX optional<NonTrivialButConstexpr> o3{33};
    o2 = o1;
    o3 = std::move(o2);
    return ((*o1).dummy == (*o3).dummy && o1->dummy == o3->dummy);
}

void g2() {
    constexpr bool b = f3();
    static_assert(b);
    constexpr bool b2 = f4();
    static_assert(b2);
}
