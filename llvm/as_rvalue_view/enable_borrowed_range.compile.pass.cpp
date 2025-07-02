// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "rxx/as_rvalue_view.h"
#include <vector>

static_assert(std::ranges::enable_borrowed_range<
              rxx::ranges::as_rvalue_view<std::ranges::empty_view<int>>>);
static_assert(
    std::ranges::enable_borrowed_range<
        rxx::ranges::as_rvalue_view<std::views::all_t<std::vector<int> &>>>);
static_assert(
    !std::ranges::enable_borrowed_range<
        rxx::ranges::as_rvalue_view<std::views::all_t<std::vector<int>>>>);
