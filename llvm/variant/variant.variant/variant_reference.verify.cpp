// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

#include "rxx/variant.h"

int main(int, char**) {
    // expected-error@variant:* 3 {{static assertion failed}}
    __RXX variant<int, int&> v; // expected-note {{requested here}}
    __RXX variant<int, int const&> v2; // expected-note {{requested here}}
    __RXX variant<int, int&&> v3; // expected-note {{requested here}}

    return 0;
}
