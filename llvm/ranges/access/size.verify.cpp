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

// __RXX ranges::size

#include "rxx/ranges/access.h"

namespace xranges = __RXX ranges;

extern int arr[];

// Verify that for an array of unknown bound `ranges::size` is ill-formed.
void test() {
    xranges::size(arr);
    // clang-format off
    // expected-error-re@-1 {{{{no matching function for call to object of type 'const (__RXX ranges::)?details::size_func_t'}}}}
    // clang-format on
}
