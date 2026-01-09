// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <ranges>

// template <class Range, class Pred>
// chunk_by_view(Range&&, Pred) -> chunk_by_view<views::all_t<Range>, Pred>;

#include "../../test_iterators.h"
#include "rxx/ranges/chunk_by_view.h"
#include "rxx/ranges/owning_view.h"
#include "rxx/ranges/ref_view.h"
#include "rxx/ranges/view_base.h"

#include <cassert>
#include <type_traits>

namespace xranges = __RXX ranges;

struct View : xranges::view_base {
    View() = default;
    forward_iterator<int*> begin() const;
    sentinel_wrapper<forward_iterator<int*>> end() const;
};
static_assert(xranges::view<View>);

// A range that is not a view
struct Range {
    Range() = default;
    forward_iterator<int*> begin() const;
    sentinel_wrapper<forward_iterator<int*>> end() const;
};
static_assert(xranges::range<Range>);
static_assert(!xranges::view<Range>);

struct Pred {
    constexpr bool operator()(int x, int y) const { return x <= y; }
};

constexpr bool test() {
    {
        View v;
        Pred pred;
        xranges::chunk_by_view view(v, pred);
        static_assert(
            std::is_same_v<decltype(view), xranges::chunk_by_view<View, Pred>>);
    }
    {
        Range r;
        Pred pred;
        xranges::chunk_by_view view(r, pred);
        static_assert(std::is_same_v<decltype(view),
            xranges::chunk_by_view<xranges::ref_view<Range>, Pred>>);
    }
    {
        Pred pred;
        xranges::chunk_by_view view(Range{}, pred);
        static_assert(std::is_same_v<decltype(view),
            xranges::chunk_by_view<xranges::owning_view<Range>, Pred>>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
