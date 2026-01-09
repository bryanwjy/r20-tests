// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// constexpr inner-iterator::inner-iterator() = default;

#include "../types.h"
#include "rxx/ranges.h"

namespace xranges = __RXX ranges;

constexpr bool test() {
    { [[maybe_unused]] InnerIterForward i; }

    { [[maybe_unused]] InnerIterInput i; }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
