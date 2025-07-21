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

// constexpr outer-iterator(Parent& parent, iterator_t<Base> current);
//   requires forward_range<Base>

#include "../types.h"
#include "rxx/ranges.h"

#include <type_traits>
#include <utility>

static_assert(!xranges::forward_range<SplitViewInput>);
static_assert(!std::is_constructible_v<OuterIterInput, SplitViewInput&,
              xranges::iterator_t<InputView>>);

constexpr bool test() {
    ForwardView input("abc");
    SplitViewForward v(std::move(input), " ");
    [[maybe_unused]] OuterIterForward i(v, input.begin());

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
