// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// template<class... Views>
// inline constexpr bool enable_borrowed_range<zip_view<Views...>> =
//      (enable_borrowed_range<Views> && ...);

#include "rxx/zip_view.h"

#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Borrowed : std::ranges::view_base {
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool std::ranges::enable_borrowed_range<Borrowed> = true;

static_assert(std::ranges::borrowed_range<Borrowed>);

struct NonBorrowed : std::ranges::view_base {
    int* begin() const;
    int* end() const;
};
static_assert(!std::ranges::borrowed_range<NonBorrowed>);

// test borrowed_range
static_assert(std::ranges::borrowed_range<xranges::zip_view<Borrowed>>);
static_assert(
    std::ranges::borrowed_range<xranges::zip_view<Borrowed, Borrowed>>);
static_assert(
    !std::ranges::borrowed_range<xranges::zip_view<Borrowed, NonBorrowed>>);
static_assert(!std::ranges::borrowed_range<xranges::zip_view<NonBorrowed>>);
static_assert(
    !std::ranges::borrowed_range<xranges::zip_view<NonBorrowed, NonBorrowed>>);
