// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2026 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/variant.h"

#include <cassert>

struct aggressive_aggregate {
    int a;
    int b;
};

struct dumbo {
    dumbo() = delete;
};

void test_emplace() {
    __RXX variant<aggressive_aggregate, dumbo> x;
    x.emplace<aggressive_aggregate>(1, 2);
    assert(x.index() == 0);
    assert(__RXX get<0>(x).a == 1);
    assert(__RXX get<0>(x).b == 2);
    x.emplace<aggressive_aggregate>(1);
    assert(x.index() == 0);
    assert(__RXX get<0>(x).a == 1);
    assert(__RXX get<0>(x).b == 0);
    x.emplace<aggressive_aggregate>();
    assert(x.index() == 0);
    assert(__RXX get<0>(x).a == 0);
    assert(__RXX get<0>(x).b == 0);
}

void test_in_place_type_construct() {
    using Var = __RXX variant<aggressive_aggregate, dumbo>;
    Var x{std::in_place_type<aggressive_aggregate>, 1, 2};
    assert(x.index() == 0);
    assert(__RXX get<0>(x).a == 1);
    assert(__RXX get<0>(x).b == 2);
    Var y{std::in_place_type<aggressive_aggregate>, 1};
    assert(y.index() == 0);
    assert(__RXX get<0>(y).a == 1);
    assert(__RXX get<0>(y).b == 0);
    Var z{std::in_place_type<aggressive_aggregate>};
    assert(z.index() == 0);
    assert(__RXX get<0>(z).a == 0);
    assert(__RXX get<0>(z).b == 0);
}

void test_in_place_index_construct() {
    using Var = __RXX variant<aggressive_aggregate, dumbo>;
    Var x{std::in_place_index<0>, 1, 2};
    assert(x.index() == 0);
    assert(__RXX get<0>(x).a == 1);
    assert(__RXX get<0>(x).b == 2);
    Var y{std::in_place_index<0>, 1};
    assert(y.index() == 0);
    assert(__RXX get<0>(y).a == 1);
    assert(__RXX get<0>(y).b == 0);
    Var z{std::in_place_index<0>};
    assert(z.index() == 0);
    assert(__RXX get<0>(z).a == 0);
    assert(__RXX get<0>(z).b == 0);
}

int main() {
    test_emplace();
    test_in_place_type_construct();
    test_in_place_index_construct();
}
