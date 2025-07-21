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

// single_view() requires default_initializable<T> = default;

#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct BigType {
    char buffer[64] = {10};
};

template <bool DefaultCtorEnabled>
struct IsDefaultConstructible {
    IsDefaultConstructible()
    requires DefaultCtorEnabled
    = default;
};

constexpr bool test() {
    static_assert(std::default_initializable<
        xranges::single_view<IsDefaultConstructible<true>>>);
    static_assert(!std::default_initializable<
                  xranges::single_view<IsDefaultConstructible<false>>>);

    {
        xranges::single_view<BigType> sv;
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }
    {
        xranges::single_view<BigType> const sv;
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
