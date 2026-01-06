// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <optional>

// A program that necessitates the instantiation of template optional for
// (possibly cv-qualified) nullopt_t is ill-formed.

#include "rxx/optional.h"

void f() {
    __RXX optional<__RXX nullopt_t>
        opt; // expected-note 1 {{requested here}}
    __RXX optional<__RXX nullopt_t const>
        opt1; // expected-note 1 {{requested here}}
    __RXX optional<__RXX nullopt_t&>
        opt2; // expected-note 1 {{requested here}}
    __RXX optional<__RXX nullopt_t&&>
        opt3; // expected-note 1 {{requested here}}
    // expected-error@optional:* 4 {{instantiation of optional with nullopt_t is
    // ill-formed}}
}
