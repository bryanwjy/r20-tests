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

// Tests that <value_> is a <copyable-box>.

#include "rxx/ranges.h"

#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct NotAssignable {
    NotAssignable() = default;
    NotAssignable(NotAssignable const&) = default;
    NotAssignable(NotAssignable&&) = default;

    NotAssignable& operator=(NotAssignable const&) = delete;
    NotAssignable& operator=(NotAssignable&&) = delete;
};

constexpr bool test() {
    xranges::single_view<NotAssignable> const a;
    xranges::single_view<NotAssignable> b;
    b = a;
    b = std::move(a);

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
