// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
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

#include "rxx/ranges/to_input_view.h"

#include "../test_iterators.h"
#include "rxx/algorithm.h"

#include <cassert>
#include <ranges>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;
namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

void test01() {
    std::vector<int> r{1, 2, 3};
    auto v = r | xviews::to_input;
    using type = decltype(v);
    static_assert(xranges::input_range<type> && !xranges::forward_range<type>);

    assert(xranges::equal(v.base(), r));
    assert(v.size() == r.size());
    assert(v.end() == r.end());
    auto it = v.begin();
    assert(it != r.end());
    *it = 42;
    ++it;
    *it = 43;
    it++;
    xranges::iter_swap(v.begin(), it);
    assert(xranges::equal(r, (int[]){3, 43, 42}));
    *it = xranges::iter_move(it);
    assert(it == r.begin() + 2);
    assert(r.end() - it == 1);
    assert(it - r.end() == -1);
}

void test02() {
    int x[] = {1, 2, 3};
    rxx::tests::test_input_range<int> rx(x);
    static_assert(!xranges::common_range<decltype(rx)>);
    auto v = rx | xviews::to_input;
    static_assert(std::same_as<decltype(v), decltype(xviews::all(rx))>);
    static_assert(std::same_as<decltype(x | xviews::to_input),
        decltype(x | xviews::to_input | xviews::to_input)>);
}

int main() {
    test01();
    test02();
}
