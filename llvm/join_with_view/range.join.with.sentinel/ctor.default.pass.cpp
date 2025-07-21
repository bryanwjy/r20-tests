// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// <ranges>

// sentinel() = default;

#include "../types.h"
#include "rxx/ranges.h"

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test() {
    using Inner = BasicVectorView<char, ViewProperties{.common = false},
        forward_iterator>;
    using V = BasicVectorView<Inner, ViewProperties{}, forward_iterator>;
    using Pattern = Inner;
    using JWV = xranges::join_with_view<V, Pattern>;
    static_assert(!xranges::common_range<JWV>);

    [[maybe_unused]] xranges::sentinel_t<JWV> se;
    [[maybe_unused]] xranges::sentinel_t<const JWV> cse;

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
