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

// Test that single_view conforms to range and view concepts.

#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <cassert>
#include <concepts>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Empty {};

static_assert(xranges::contiguous_range<xranges::single_view<Empty>>);
static_assert(xranges::contiguous_range<xranges::single_view<Empty> const>);
static_assert(xranges::view<xranges::single_view<Empty>>);
static_assert(xranges::view<xranges::single_view<Empty const>>);
static_assert(
    xranges::contiguous_range<xranges::single_view<Empty const> const>);
static_assert(xranges::view<xranges::single_view<int>>);
static_assert(xranges::view<xranges::single_view<int const>>);
static_assert(xranges::contiguous_range<xranges::single_view<int const> const>);
