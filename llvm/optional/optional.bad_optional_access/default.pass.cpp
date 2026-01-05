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

// <optional>

// class bad_optional_access is default constructible

#include "rxx/optional.h"

int main(int, char**) {
    using __RXX bad_optional_access;
    bad_optional_access ex;

    return 0;
}
