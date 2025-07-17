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

// rxx::ranges::end

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/access.h"

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct NonBorrowedRange {
    int* begin() const;
    int* end() const;
};
static_assert(!std::ranges::enable_borrowed_range<NonBorrowedRange>);

// Verify that if the expression is an rvalue and `enable_borrowed_range` is
// false, `ranges::end` is ill-formed.
void test() {
    xranges::end(NonBorrowedRange());
    // clang-format off
    // expected-error-re@-1 {{{{call to deleted function call operator in type 'const (rxx::ranges::)?details::end_t'}}}}
    // expected-error@-2  {{attempt to use a deleted function}}
    // clang-format on
}
