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

#include "rxx/ranges/as_rvalue_view.h"
#include <ranges>
#include <utility>
#include <vector>

static_assert(
    std::is_same_v<
        decltype(rxx::ranges::as_rvalue_view(std::vector<int>{})),
        rxx::ranges::as_rvalue_view<std::views::all_t<std::vector<int>>>>);

static_assert(
    std::is_same_v<
        decltype(rxx::ranges::as_rvalue_view(
            std::declval<std::vector<int> &>())),
        rxx::ranges::as_rvalue_view<std::views::all_t<std::vector<int> &>>>);

static_assert(
    std::is_same_v<
        decltype(rxx::ranges::as_rvalue_view(std::ranges::empty_view<int>{})),
        rxx::ranges::as_rvalue_view<std::ranges::empty_view<int>>>);
