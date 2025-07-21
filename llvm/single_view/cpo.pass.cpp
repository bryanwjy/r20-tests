// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// xviews::single

#include "../MoveOnly.h"
#include "rxx/ranges.h"

#include <cassert>
#include <concepts>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

// Can't invoke without arguments.
static_assert(!std::is_invocable_v<decltype((xviews::single))>);
static_assert(std::is_invocable_v<decltype((xviews::single)), MoveOnly>);

constexpr bool test() {
    // Lvalue.
    {
        int x = 42;
        std::same_as<xranges::single_view<int>> decltype(auto) v =
            xviews::single(x);
        assert(v.size() == 1);
        assert(v.front() == x);
    }

    // Prvalue.
    {
        std::same_as<xranges::single_view<int>> decltype(auto) v =
            xviews::single(42);
        assert(v.size() == 1);
        assert(v.front() == 42);
    }

    // Const lvalue.
    {
        int const x = 42;
        std::same_as<xranges::single_view<int>> decltype(auto) v =
            xviews::single(x);
        assert(v.size() == 1);
        assert(v.front() == x);
    }

    // Xvalue.
    {
        int x = 42;
        std::same_as<xranges::single_view<int>> decltype(auto) v =
            xviews::single(std::move(x));
        assert(v.size() == 1);
        assert(v.front() == x);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
