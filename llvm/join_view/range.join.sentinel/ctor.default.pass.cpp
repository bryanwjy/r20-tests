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

// sentinel() = default;

#include "../types.h"
#include "rxx/join_view.h"

#include <cassert>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
constexpr bool test() {
    xranges::sentinel_t<xranges::join_view<CopyableParent>> sent;
    (void)sent;

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
