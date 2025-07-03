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

// explicit rxx::ranges::lazy_split_view::outer-iterator::outer-iterator(Parent&
// parent)
//   requires (!forward_range<Base>)

#include "../types.h"
#include "rxx/lazy_split_view.h"

#include <type_traits>
#include <utility>

// Verify that the constructor is `explicit`.
static_assert(!std::is_convertible_v<SplitViewInput&, OuterIterInput>);

static_assert(std::ranges::forward_range<SplitViewForward>);
static_assert(!std::is_constructible_v<OuterIterForward, SplitViewForward&>);

constexpr bool test() {
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
