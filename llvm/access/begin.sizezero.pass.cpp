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
// UNSUPPORTED: msvc

// std::ranges::begin
// std::ranges::cbegin
//   Test the fix for https://llvm.org/PR54100

#include "../static_asserts.h"
#include "../test_iterators.h"
#include "rxx/access.h"

#include <cassert>

#if RXX_COMPILER_CLANG | RXX_COMPILER_GCC

struct A {
    int m[0];
};
static_assert(sizeof(A) == 0); // an extension supported by GCC and Clang

int main(int, char**) {
    A a[10];
    std::same_as<A*> auto p = std::ranges::begin(a);
    assert(p == a);
    std::same_as<A const*> auto cp = std::ranges::cbegin(a);
    assert(cp == a);

    return 0;
}
#else

int main() {
    return 0;
}

#endif
