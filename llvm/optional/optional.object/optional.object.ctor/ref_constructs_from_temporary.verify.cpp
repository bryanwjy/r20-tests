// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++26

// optional

#include "rxx/optional.h"

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
#  include <utility>

struct X {
    int i;

    X(int j) : i(j) {}
};

int main(int, char**) {
    __RXX optional<int> const _co(1);
    __RXX optional<int> _o(1);

    // expected-error-re@*:* 8 {{call to deleted constructor of '__RXX
    // optional<{{.*}}>'}}
    __RXX optional<int const&> o1{1}; // optional(U&&)
    __RXX optional<int const&> o2{
        __RXX optional<int>(1)}; // optional(optional<U>&&)
    // __RXX optional<int const&> o3{_co}; // optional(const optional<U>&)
    // __RXX optional<int const&> o4{_o}; // optional(optional<U>&)
    __RXX optional<X const&> o5{1}; // optional(U&&)
    __RXX optional<X const&> o6{
        __RXX optional<int>(1)}; // optional(optional<U>&&)
    __RXX optional<X const&> o7{_co}; // optional(const optional<U>&)
    __RXX optional<X const&> o8{_o}; // optional(optional<U>&)
}
#endif
