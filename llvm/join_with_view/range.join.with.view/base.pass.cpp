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

// constexpr View base() const& requires copy_constructible<View>;
// constexpr View base() &&;

#include "rxx/ranges.h"

#include <cassert>
#include <utility>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

using InnerRange = std::vector<int>;

struct Range : xranges::view_base {
    constexpr explicit Range(InnerRange* b, InnerRange* e)
        : begin_(b)
        , end_(e) {}
    constexpr Range(Range const& other)
        : begin_(other.begin_)
        , end_(other.end_)
        , was_copy_initialized_(true) {}
    constexpr Range(Range&& other)
        : begin_(other.begin_)
        , end_(other.end_)
        , was_move_initialized_(true) {}
    Range& operator=(Range const&) = default;
    Range& operator=(Range&&) = default;
    constexpr InnerRange* begin() const { return begin_; }
    constexpr InnerRange* end() const { return end_; }

    InnerRange* begin_;
    InnerRange* end_;
    bool was_copy_initialized_ = false;
    bool was_move_initialized_ = false;
};

static_assert(xranges::view<Range>);
static_assert(xranges::input_range<Range>);

struct Pattern : xranges::view_base {
    static constexpr int pat[2] = {0, 0};
    constexpr int const* begin() const { return pat; }
    constexpr int const* end() const { return pat + 2; }
};

static_assert(xranges::view<Pattern>);
static_assert(xranges::forward_range<Pattern>);

template <class Tp>
struct NonCopyableRange : xranges::view_base {
    NonCopyableRange(NonCopyableRange const&) = delete;
    NonCopyableRange(NonCopyableRange&&) = default;
    NonCopyableRange& operator=(NonCopyableRange const&) = default;
    NonCopyableRange& operator=(NonCopyableRange&&) = default;
    Tp* begin() const;
    Tp* end() const;
};

static_assert(!std::copy_constructible<NonCopyableRange<InnerRange>>);
static_assert(!std::copy_constructible<NonCopyableRange<int>>);

template <typename T>
concept CanCallBaseOn = requires(T&& t) { std::forward<T>(t).base(); };

constexpr bool test() {
    InnerRange buff[3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    Pattern pattern;

    { // Check the const& overload
        Range range(buff, buff + 3);
        xranges::join_with_view<Range, Pattern> view(range, pattern);
        std::same_as<Range> decltype(auto) result = view.base();
        assert(result.was_copy_initialized_);
        assert(result.begin() == buff);
        assert(result.end() == buff + 3);
    }

    { // Check the const& overload on const `view`
        Range range(buff, buff + 3);
        xranges::join_with_view<Range, Pattern> const view(range, pattern);
        std::same_as<Range> decltype(auto) result = view.base();
        assert(result.was_copy_initialized_);
        assert(result.begin() == buff);
        assert(result.end() == buff + 3);
    }

    { // Check the && overload
        Range range(buff, buff + 3);
        xranges::join_with_view<Range, Pattern> view(range, pattern);
        std::same_as<Range> decltype(auto) result = std::move(view).base();
        assert(result.was_move_initialized_);
        assert(result.begin() == buff);
        assert(result.end() == buff + 3);
    }

    { // Ensure the const& overload is not considered when the base is not
      // copy-constructible
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                Pattern> const&>);
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                Pattern>&>);
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                Pattern> const&&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<NonCopyableRange<InnerRange>, Pattern>&&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<NonCopyableRange<InnerRange>, Pattern>>);
    }

    { // Ensure the const& overload does not depend on Pattern's
      // copy-constructability
        static_assert(CanCallBaseOn<
            xranges::join_with_view<Range, NonCopyableRange<int>> const&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<Range, NonCopyableRange<int>>&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<Range, NonCopyableRange<int>> const&&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<Range, NonCopyableRange<int>>&&>);
        static_assert(CanCallBaseOn<
            xranges::join_with_view<Range, NonCopyableRange<int>>>);
    }

    { // Check above two at the same time
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                NonCopyableRange<int>> const&>);
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                NonCopyableRange<int>>&>);
        static_assert(
            !CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                NonCopyableRange<int>> const&&>);
        static_assert(
            CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                NonCopyableRange<int>>&&>);
        static_assert(
            CanCallBaseOn<xranges::join_with_view<NonCopyableRange<InnerRange>,
                NonCopyableRange<int>>>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
