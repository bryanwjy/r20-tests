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

// rxx::ranges::size

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/ranges/access.h"

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

extern int arr[];

// Verify that for an array of unknown bound `ranges::size` is ill-formed.
void test() {
    xranges::size(arr);
    // clang-format off
    // expected-error-re@-1 {{{{no matching function for call to object of type 'const (rxx::ranges::)?details::size_func_t'}}}}
    // clang-format on
}
