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

// single_view does not specialize enable_borrowed_range

#include "../test_range.h"
#include "rxx/ranges/single_view.h"

#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

static_assert(!xranges::borrowed_range<xranges::single_view<int>>);
static_assert(!xranges::borrowed_range<xranges::single_view<int*>>);
static_assert(!xranges::borrowed_range<xranges::single_view<BorrowedView>>);
static_assert(!xranges::borrowed_range<xranges::single_view<NonBorrowedView>>);
