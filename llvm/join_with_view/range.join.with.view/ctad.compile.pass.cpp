// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// <ranges>

// template<class R, class P>
//   join_with_view(R&&, P&&) -> join_with_view<views::all_t<R>,
//   views::all_t<P>>;
//
// template<input_range R>
//   join_with_view(R&&, range_value_t<range_reference_t<R>>)
//     -> join_with_view<views::all_t<R>,
//     single_view<range_value_t<range_reference_t<R>>>>;

#include "../../test_iterators.h"
#include "rxx/ranges.h"

#include <deque>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct View : std::ranges::view_base {
    using It = cpp20_input_iterator<std::deque<int>*>;

    View() = default;
    It begin() const;
    sentinel_wrapper<It> end() const;
};

static_assert(xranges::input_range<View>);
static_assert(xranges::view<View>);

struct Pattern : std::ranges::view_base {
    Pattern() = default;
    forward_iterator<int*> begin();
    forward_iterator<int*> end();
};

static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);

// A range that is not a view
struct Range {
    using It = cpp20_input_iterator<std::deque<int>*>;

    Range() = default;
    It begin() const;
    sentinel_wrapper<It> end() const;
};

static_assert(xranges::input_range<Range>);
static_assert(!xranges::view<Range>);

// A pattern that is not a view
struct RangePattern {
    RangePattern() = default;
    forward_iterator<int*> begin();
    forward_iterator<int*> end();
};

static_assert(xranges::forward_range<RangePattern>);
static_assert(!xranges::view<RangePattern>);

void test_range_and_pattern_deduction_guide() {
    { // Both `v` and `pat` model `std::ranges::view`.
        {
            View v;
            Pattern pat;
            xranges::join_with_view view(v, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, Pattern>>);
        }
        {
            View v;
            xranges::join_with_view view(v, Pattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, Pattern>>);
        }
        {
            Pattern pat;
            xranges::join_with_view view(View{}, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, Pattern>>);
        }
        {
            xranges::join_with_view view(View{}, Pattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, Pattern>>);
        }
    }

    { // Only `pat` models `std::ranges::view`.
        {
            Range v;
            Pattern pat;
            xranges::join_with_view view(v, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    Pattern>>);
        }
        {
            Range v;
            xranges::join_with_view view(v, Pattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    Pattern>>);
        }
        {
            Pattern pat;
            xranges::join_with_view view(Range{}, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    Pattern>>);
        }
        {
            xranges::join_with_view view(Range{}, Pattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    Pattern>>);
        }
    }

    { // Only `v` models `std::ranges::view`.
        {
            View v;
            RangePattern pat;
            xranges::join_with_view view(v, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View,
                    std::ranges::ref_view<RangePattern>>>);
        }
        {
            View v;
            xranges::join_with_view view(v, RangePattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View,
                    std::ranges::owning_view<RangePattern>>>);
        }
        {
            RangePattern pat;
            xranges::join_with_view view(View{}, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View,
                    std::ranges::ref_view<RangePattern>>>);
        }
        {
            xranges::join_with_view view(View{}, RangePattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View,
                    std::ranges::owning_view<RangePattern>>>);
        }
    }

    { // Both `v` and `pat` don't model `std::ranges::view`.
        {
            Range r;
            RangePattern pat;
            xranges::join_with_view view(r, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    std::ranges::ref_view<RangePattern>>>);
        }
        {
            Range r;
            xranges::join_with_view view(r, RangePattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    std::ranges::owning_view<RangePattern>>>);
        }
        {
            RangePattern pat;
            xranges::join_with_view view(Range{}, pat);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    std::ranges::ref_view<RangePattern>>>);
        }
        {
            xranges::join_with_view view(Range{}, RangePattern{});
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    std::ranges::owning_view<RangePattern>>>);
        }
    }
}

void test_range_and_element_deduction_guide() {
    { // Element is lvalue
        int elem = 0;

        {
            View v;
            xranges::join_with_view view(v, elem);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, xranges::single_view<int>>>);
        }
        {
            xranges::join_with_view view(View{}, elem);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, xranges::single_view<int>>>);
        }
        {
            Range r;
            xranges::join_with_view view(r, elem);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    xranges::single_view<int>>>);
        }
        {
            xranges::join_with_view view(Range{}, elem);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    xranges::single_view<int>>>);
        }
    }

    { // Element is rvalue
        {
            View v;
            xranges::join_with_view view(v, 1);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, xranges::single_view<int>>>);
        }
        {
            xranges::join_with_view view(View{}, 1);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<View, xranges::single_view<int>>>);
        }
        {
            Range r;
            xranges::join_with_view view(r, 1);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::ref_view<Range>,
                    xranges::single_view<int>>>);
        }
        {
            xranges::join_with_view view(Range{}, 1);
            static_assert(std::is_same_v<decltype(view),
                xranges::join_with_view<std::ranges::owning_view<Range>,
                    xranges::single_view<int>>>);
        }
    }
}
