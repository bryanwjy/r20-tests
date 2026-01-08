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

// template <class T>
// class optional
// {
// public:
//     typedef T value_type;
//     ...

#include "rxx/optional.h"

#include <type_traits>

using __RXX optional;

template <class Opt, class T>
void test() {
    static_assert(std::is_same<typename Opt::value_type, T>::value, "");
}

int main(int, char**) {
    test<optional<int>, int>();
    test<optional<int const>, int const>();
    test<optional<double>, double>();
    test<optional<double const>, double const>();
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    test<optional<int&>, int>();
    test<optional<int const&>, int const>();
    test<optional<double&>, double>();
    test<optional<double const&>, double const>();
#endif
    return 0;
}
