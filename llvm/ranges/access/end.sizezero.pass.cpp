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
// UNSUPPORTED: msvc

// xranges::end
// xranges::cend
//   Test the fix for https://llvm.org/PR54100

#include "../../static_asserts.h"
#include "../../test_iterators.h"
#include "rxx/ranges/access.h"

#include <cassert>

#if RXX_COMPILER_CLANG | RXX_COMPILER_GCC

namespace xranges = __RXX ranges;

struct A {
    int m[0];
};
static_assert(sizeof(A) == 0); // an extension supported by GCC and Clang

int main() {
    A a[10];
    std::same_as<A*> auto p = xranges::end(a);
    assert(p == a + 10);
    std::same_as<A const*> auto cp = xranges::cend(a);
    assert(cp == a + 10);

    return 0;
}
#else

int main() {
    return 0;
}
#endif
