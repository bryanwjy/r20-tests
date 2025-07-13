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
// UNSUPPORTED: no-localization

// Check LWG-3698: `regex_iterator` and `join_view` don't work together very
// well

#include "rxx/join_view.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <regex>
#include <string_view>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

int main(int, char**) {
    // This test requires p2770r0 to be implemented in <regex>
#if 0
    char const text[] = "Hello";
    std::regex regex{"[a-z]"};

    auto lower =
        std::ranges::subrange(std::cregex_iterator(xranges::begin(text),
                                  xranges::end(text), regex),
            std::cregex_iterator{}) |
        xviews::join | std::views::transform([](auto const& sm) {
            return std::string_view(sm.first, sm.second);
        });

    assert(std::ranges::equal(
        lower, std::to_array<std::string_view>({"e", "l", "l", "o"})));
#endif
    return 0;
}
