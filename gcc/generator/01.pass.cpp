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

#if RXX_SUPPORTS_GENERATOR
#  include "rxx/ranges.h"

#  include <cassert>
#  include <span>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;
template <typename... Ts>
using xgenerator = __RXX generator<Ts...>;

constexpr std::array test_sequence{1, 2, 3, 4, 5};
constexpr struct {
    int offset, count;
} bar_subsequence{.offset = 2, .count = 2};

// basic example
xgenerator<int> bar() {
    for (auto const val : std::span{test_sequence}
             .subspan<bar_subsequence.offset, bar_subsequence.count>()) {
        co_yield val;
    }
}

xgenerator<int> foo() {
    for (auto const val :
        std::span{test_sequence}.first<bar_subsequence.offset>()) {
        co_yield val;
    }
    co_yield xranges::elements_of{bar()};
    for (auto const val : std::span{test_sequence}
             .subspan<bar_subsequence.offset + bar_subsequence.count>()) {
        co_yield val;
    }
}

int main() {
    for (auto [a, x] : xviews::zip(foo(), test_sequence)) {
        assert(a == x);
    }
}
#else
int main() {
    return 0;
}
#endif
