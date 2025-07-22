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

// template<input_range R>
//   requires constructible_from<V, views::all_t<R>> &&
//   constructible_from<Pattern, single_view<range_value_t<InnerRng>>>
// constexpr explicit join_with_view(R&& r, range_value_t<InnerRng> e);

#include "../../test_iterators.h"
#include "../../test_range.h"
#include "../types.h"
#include "rxx/algorithm.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct MoveOnlyInt {
    MoveOnlyInt() = default;
    MoveOnlyInt(MoveOnlyInt&&) = default;
    MoveOnlyInt& operator=(MoveOnlyInt&&) = default;

    constexpr MoveOnlyInt(int val) : val_(val) {}
    constexpr operator int() const { return val_; }

    int val_ = 0;
};

template <>
struct std::common_type<MoveOnlyInt, int> {
    using type = int;
};

template <>
struct std::common_type<int, MoveOnlyInt> {
    using type = int;
};

struct OutputView : xranges::view_base {
    using It = cpp20_output_iterator<int*>;
    It begin() const;
    sentinel_wrapper<It> end() const;
};

static_assert(xranges::output_range<OutputView, int>);
static_assert(xranges::view<OutputView>);

struct InputRange {
    using It = cpp20_input_iterator<int*>;
    It begin() const;
    sentinel_wrapper<It> end() const;
};

struct InputView : InputRange, xranges::view_base {};

static_assert(xranges::input_range<InputRange>);
static_assert(xranges::input_range<InputRange const>);
static_assert(xranges::view<InputView>);
static_assert(xranges::input_range<InputView>);
static_assert(xranges::input_range<InputView const>);

class View : public xranges::view_base {
    using OuterRange = std::array<std::array<MoveOnlyInt, 2>, 3>;

    static constexpr OuterRange range_on_input_view = {
        {{1, 1}, {1, 1}, {1, 1}}
    };
    static constexpr OuterRange range_on_ref_input_range = {
        {{2, 2}, {2, 2}, {2, 2}}
    };
    static constexpr OuterRange range_on_const_ref_input_range = {
        {{3, 3}, {3, 3}, {3, 3}}
    };
    static constexpr OuterRange range_on_owning_input_range = {
        {{4, 4}, {4, 4}, {4, 4}}
    };

    OuterRange const* r_;

public:
    // Those functions should never be called in this test.
    View(View&&) { assert(false); }
    View(OutputView) { assert(false); }
    View& operator=(View&&) {
        assert(false);
        return *this;
    }

    constexpr explicit View(InputView) : r_(&range_on_input_view) {}
    constexpr explicit View(InputRange) = delete;
    constexpr explicit View(xranges::ref_view<InputRange>)
        : r_(&range_on_ref_input_range) {}
    constexpr explicit View(xranges::ref_view<InputRange const>)
        : r_(&range_on_const_ref_input_range) {}
    constexpr explicit View(xranges::owning_view<InputRange>)
        : r_(&range_on_owning_input_range) {}

    constexpr auto begin() const { return r_->begin(); }
    constexpr auto end() const { return r_->end(); }
};

static_assert(xranges::input_range<View>);
static_assert(xranges::input_range<View const>);

class Pattern : public xranges::view_base {
    int val_;

public:
    // Those functions should never be called in this test.
    Pattern(Pattern&&) { assert(false); }
    template <class T>
    Pattern(xranges::single_view<T> const&) {
        assert(false);
    }
    Pattern& operator=(Pattern&&) {
        assert(false);
        return *this;
    }

    template <class T>
    constexpr explicit Pattern(xranges::single_view<T>&& v) : val_(v[0]) {}

    constexpr int const* begin() const { return &val_; }
    constexpr int const* end() const { return &val_ + 1; }
};

static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::forward_range<Pattern const>);

constexpr void test_ctor_with_view_and_element() {
    // Check construction from `r` and `e`, when `r` models `xranges::view`

    { // `r` and `e` are glvalues
        InputView r;
        static_assert(
            std::constructible_from<View, xviews::all_t<InputView&>> &&
            std::constructible_from<Pattern,
                xranges::single_view<
                    xranges::range_value_t<xranges::range_reference_t<View>>>>);
        int e = 0;
        xranges::join_with_view<View, Pattern> jwv(r, e);
        assert(xranges::equal(jwv, std::array{1, 1, 0, 1, 1, 0, 1, 1}));
    }

    { // `r` and `e` are const glvalues
        InputView const r;
        int const e = 1;
        xranges::join_with_view<View, Pattern> jwv(r, e);
        assert(xranges::equal(jwv, std::array{1, 1, 1, 1, 1, 1, 1, 1}));
    }

    { // `r` and `e` are prvalues
        xranges::join_with_view<View, Pattern> jwv(InputView{}, MoveOnlyInt{2});
        assert(xranges::equal(jwv, std::array{1, 1, 2, 1, 1, 2, 1, 1}));
    }

    { // `r` and `e` are xvalues
        InputView r;
        MoveOnlyInt e = 3;
        xranges::join_with_view<View, Pattern> jwv(std::move(r), std::move(e));
        assert(xranges::equal(jwv, std::array{1, 1, 3, 1, 1, 3, 1, 1}));
    }

    // Check explicitness
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputView, MoveOnlyInt>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputView, int>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputView&, int&>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputView const, int const>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputView const&, int const&>);
}

constexpr void test_ctor_with_non_view_and_element() {
    // Check construction from `r` and `e`, when `r` does not model
    // `xranges::view`

    { // `r` and `e` are glvalues
        InputRange r;
        int e = 0;
        xranges::join_with_view<View, Pattern> jwv(r, e);
        assert(xranges::equal(jwv, std::array{2, 2, 0, 2, 2, 0, 2, 2}));
    }

    { // `r` and `e` are const glvalues
        InputRange const r;
        int const e = 1;
        xranges::join_with_view<View, Pattern> jwv(r, e);
        assert(xranges::equal(jwv, std::array{3, 3, 1, 3, 3, 1, 3, 3}));
    }

    { // `r` and `e` are prvalues
        xranges::join_with_view<View, Pattern> jwv(
            InputRange{}, MoveOnlyInt{2});
        assert(xranges::equal(jwv, std::array{4, 4, 2, 4, 4, 2, 4, 4}));
    }

    { // `r` and `e` are xvalues
        InputRange r;
        MoveOnlyInt e = 3;
        xranges::join_with_view<View, Pattern> jwv(std::move(r), std::move(e));
        assert(xranges::equal(jwv, std::array{4, 4, 3, 4, 4, 3, 4, 4}));
    }

    // Check explicitness
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputRange, MoveOnlyInt>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputRange, int>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputRange&, int&>);
    static_assert(ConstructionIsExplicit<xranges::join_with_view<View, Pattern>,
        InputRange const&, int const&>);
}

constexpr void test_constraints() {
    { // `R` is not an input range
        using R = OutputView;
        static_assert(!xranges::input_range<R>);
        static_assert(std::constructible_from<View, xviews::all_t<R>>);
        static_assert(
            std::constructible_from<Pattern, xranges::single_view<int>>);
        static_assert(
            !std::constructible_from<xranges::join_with_view<View, Pattern>, R,
                int>);
    }

    { // `V` is not constructible from `views::all_t<R>`
        using R = test_range<cpp20_input_iterator>;
        static_assert(xranges::input_range<R>);
        static_assert(!std::constructible_from<View, xviews::all_t<R>>);
        static_assert(
            std::constructible_from<Pattern, xranges::single_view<int>>);
        static_assert(
            !std::constructible_from<xranges::join_with_view<View, Pattern>, R,
                int>);
    }

    { // `Pattern` is not constructible from
      // `single_view<range_value_t<InnerRng>>`
        using R = InputView;
        using Pat = test_view<forward_iterator>;
        static_assert(xranges::input_range<R>);
        static_assert(std::constructible_from<View, xviews::all_t<R>>);
        static_assert(!std::constructible_from<Pat, xranges::single_view<int>>);
        static_assert(
            !std::constructible_from<xranges::join_with_view<View, Pat>, R,
                int>);
    }
}

constexpr bool test() {
    test_ctor_with_view_and_element();
    test_ctor_with_non_view_and_element();
    test_constraints();

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
