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

// template<class T>
//   single_view(T) -> single_view<T>;

#include "../test_iterators.h"
#include "rxx/single_view.h"

#include <cassert>
#include <concepts>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Empty {};

static_assert(std::same_as<decltype(xranges::single_view(Empty())),
    xranges::single_view<Empty>>);

static_assert(
    std::same_as<decltype(xranges::single_view(std::declval<Empty&>())),
        xranges::single_view<Empty>>);

static_assert(
    std::same_as<decltype(xranges::single_view(std::declval<Empty&&>())),
        xranges::single_view<Empty>>);
