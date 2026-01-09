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

// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/generator.h"
#if RXX_SUPPORTS_GENERATOR
#  include "rxx/ranges.h"

#  include <array>
#  include <cassert>

namespace xranges = __RXX ranges;

template <typename... Ts>
using xgenerator = __RXX generator<Ts...>;
#  include <vector>

xgenerator<int&> yield_vector() {
    std::vector foo{1, 2, 3};
    auto x = 123;
    co_yield x;
    co_yield xranges::elements_of{foo};
    x = 456;
    co_yield x;
}

int main() {
    constexpr std::array expected{123, 1, 2, 3, 456};
    for (auto [a, e] : __RXX views::zip(yield_vector(), expected)) {
        assert(a == e);
    }
}
#else
int main() {
    return 0;
}
#endif
