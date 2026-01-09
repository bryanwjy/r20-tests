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

// class bad_optional_access : public exception

#include "rxx/optional.h"

#include <type_traits>

int main(int, char**) {
    using __RXX bad_optional_access;

    static_assert(
        std::is_base_of<std::exception, bad_optional_access>::value, "");
    static_assert(
        std::is_convertible<bad_optional_access*, std::exception*>::value, "");

    return 0;
}
