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

// constexpr lazy_split_view(View base, Pattern pattern); // explicit since
// C++23

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <string_view>
#include <utility>

struct ViewWithCounting : xranges::view_base {
    int* times_copied = nullptr;
    int* times_moved = nullptr;

    constexpr ViewWithCounting(int& copies_ctr, int& moves_ctr)
        : times_copied(&copies_ctr)
        , times_moved(&moves_ctr) {}

    constexpr ViewWithCounting(ViewWithCounting const& rhs)
        : times_copied(rhs.times_copied)
        , times_moved(rhs.times_moved) {
        ++(*times_copied);
    }
    constexpr ViewWithCounting(ViewWithCounting&& rhs)
        : times_copied(rhs.times_copied)
        , times_moved(rhs.times_moved) {
        ++(*times_moved);
    }

    constexpr char const* begin() const { return nullptr; }
    constexpr char const* end() const { return nullptr; }

    constexpr ViewWithCounting& operator=(ViewWithCounting const&) = default;
    constexpr ViewWithCounting& operator=(ViewWithCounting&&) = default;
    constexpr bool operator==(ViewWithCounting const&) const { return true; }
};

static_assert(xranges::forward_range<ViewWithCounting>);
static_assert(xranges::view<ViewWithCounting>);

using View = ViewWithCounting;
using Pattern = ViewWithCounting;

// SFINAE tests.

static_assert(
    is_explicit_constructible_v<xranges::lazy_split_view<View, Pattern>, View,
        Pattern>,
    "This constructor must be explicit");

constexpr bool test() {
    // Calling the constructor with `(ForwardView, ForwardView)`.
    {
        CopyableView input = "abc def";
        xranges::lazy_split_view<CopyableView, CopyableView> v(input, " ");
        assert(v.base() == input);
    }

    // Calling the constructor with `(InputView, TinyView)`.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (!std::is_constant_evaluated())
#endif
    {
        InputView input = "abc def";
        xranges::lazy_split_view<InputView, ForwardTinyView> v(input, ' ');
        // Note: `InputView` isn't equality comparable.
        (void)v;
    }

    // Make sure the arguments are moved, not copied.
    {
        // Arguments are lvalues.
        {
            int view_copied = 0, view_moved = 0, pattern_copied = 0,
                pattern_moved = 0;
            View view(view_copied, view_moved);
            Pattern pattern(pattern_copied, pattern_moved);

            xranges::lazy_split_view<View, Pattern> v(view, pattern);
            assert(view_copied ==
                1); // The local variable is copied into the argument.
            assert(view_moved == 1);
            assert(pattern_copied == 1);
            assert(pattern_moved == 1);
        }

        // Arguments are rvalues.
        {
            int view_copied = 0, view_moved = 0, pattern_copied = 0,
                pattern_moved = 0;
            xranges::lazy_split_view<View, Pattern> v(
                View(view_copied, view_moved),
                Pattern(pattern_copied, pattern_moved));
            assert(view_copied == 0);
            assert(view_moved == 1);
            assert(pattern_copied == 0);
            assert(pattern_moved == 1);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
