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

// optional

#include "rxx/optional.h"

#include <iterator>

static_assert(!std::indirectly_readable<__RXX optional<int>>);
static_assert(!std::indirectly_writable<__RXX optional<int>, int>);
static_assert(!std::weakly_incrementable<__RXX optional<int>>);
static_assert(
    !std::indirectly_movable<__RXX optional<int>, __RXX optional<int>>);
static_assert(!std::indirectly_movable_storable<__RXX optional<int>,
    __RXX optional<int>>);
static_assert(
    !std::indirectly_copyable<__RXX optional<int>, __RXX optional<int>>);
static_assert(!std::indirectly_copyable_storable<__RXX optional<int>,
    __RXX optional<int>>);
