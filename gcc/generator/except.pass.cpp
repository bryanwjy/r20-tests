// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2023-2025 Free Software Foundation, Inc.
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

#include "rxx/generator.h"

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
template <typename... Ts>
using xgenerator = rxx::generator<Ts...>;

xgenerator<int> foo() {
    co_yield 0;
    throw 3; /* dice roll */
}

xgenerator<int> foo_delegator() {
    co_yield 1;
    co_yield xranges::elements_of{foo()};
}

bool catchy_caught = false;

xgenerator<int> foo_catchy_delegator() {
    try {
        co_yield xranges::elements_of{foo()};
        assert(false);
    } catch (int i) {
        catchy_caught = true;
        assert(i == 3);
    }
}

int main() {
    {
        auto gen = foo();
        try {
            auto it = gen.begin();
            assert(*it == 0);
            it++;
            assert(false);
        } catch (int x) {
            assert(x == 3);
        }
    }

    {
        auto gen = foo_delegator();
        auto it = gen.begin();
        assert(*it == 1);
        it++;

        try {
            assert(*it == 0);
            it++;
            assert(false);
        } catch (int x) {
            assert(x == 3);
        }
    }

    for (auto x : foo_catchy_delegator())
        assert(x == 0);
    assert(catchy_caught);
}
