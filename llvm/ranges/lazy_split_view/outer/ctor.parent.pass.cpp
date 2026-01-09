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

// explicit xranges::lazy_split_view::outer-iterator::outer-iterator(Parent&
// parent)
//   requires (!forward_range<Base>)

#include "../types.h"
#include "rxx/ranges.h"

#include <type_traits>
#include <utility>

// Verify that the constructor is `explicit`.
static_assert(!std::is_convertible_v<SplitViewInput&, OuterIterInput>);

static_assert(xranges::forward_range<SplitViewForward>);
static_assert(!std::is_constructible_v<OuterIterForward, SplitViewForward&>);

constexpr bool test() {
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (std::is_constant_evaluated())
        return true;
#endif
    InputView input;
    SplitViewInput v(input, ForwardTinyView());
    [[maybe_unused]] OuterIterInput i(v);

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
