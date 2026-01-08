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

// T shall be an object type other than cv in_place_t or cv nullopt_t
//   and shall satisfy the Cpp17Destructible requirements.
// Note: array types do not satisfy the Cpp17Destructible requirements.

#include "rxx/optional.h"

#include <cassert>

struct NonDestructible {
    ~NonDestructible() = delete;
};

int main(int, char**) {
    // clang-format off
  {
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    __RXX optional<int&&> opt2; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with an rvalue reference type is ill-formed}}
#else
    __RXX optional<char&> o1; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with a reference type is ill-formed}}
#endif
    __RXX optional<NonDestructible> o2; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with a non-destructible type is ill-formed}}
    __RXX optional<char[20]> o3; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with an array type is ill-formed}}
  }

  {
    __RXX optional<               std::in_place_t> o1; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with in_place_t is ill-formed}}
    __RXX optional<const          std::in_place_t> o2; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with in_place_t is ill-formed}}
    __RXX optional<      volatile std::in_place_t> o3; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with in_place_t is ill-formed}}
    __RXX optional<const volatile std::in_place_t> o4; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with in_place_t is ill-formed}}
  }

  {
    __RXX optional<               std::nullopt_t> o1; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with nullopt_t is ill-formed}}
    __RXX optional<const          std::nullopt_t> o2; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with nullopt_t is ill-formed}}
    __RXX optional<      volatile std::nullopt_t> o3; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with nullopt_t is ill-formed}}
    __RXX optional<const volatile std::nullopt_t> o4; // expected-error-re@optional:* {{static assertion failed{{.*}}instantiation of optional with nullopt_t is ill-formed}}
  }
    // clang-format on
    return 0;
}
