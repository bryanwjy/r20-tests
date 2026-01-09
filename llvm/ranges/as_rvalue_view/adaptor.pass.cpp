// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// xviews::as_rvalue

#include "../../test_iterators.h"
#include "rxx/ranges/as_rvalue_view.h"
#include "rxx/ranges/subrange.h"
#include "rxx/ranges/transform_view.h"
#include "rxx/ranges/view_base.h"

#include <cassert>
#include <functional>
#include <ranges>
#include <vector>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

struct DefaultConstructibleView : xranges::view_base {
    int i_;
    int* begin();
    int* end();
};

struct RValueView : xranges::view_base {};

template <class View, class T>
concept HasPipe = requires(View&& view, T&& t) {
    { std::forward<View>(view) | std::forward<T>(t) };
};

struct NoView {};
static_assert(
    std::is_invocable_v<decltype(xviews::as_rvalue), DefaultConstructibleView>);
static_assert(!std::is_invocable_v<decltype(xviews::as_rvalue)>);
static_assert(!std::is_invocable_v<decltype(xviews::as_rvalue), NoView>);
static_assert(HasPipe<DefaultConstructibleView&, decltype(xviews::as_rvalue)>);
static_assert(HasPipe<int (&)[10], decltype(xviews::as_rvalue)>);
static_assert(!HasPipe<int (&&)[10], decltype(xviews::as_rvalue)>);
static_assert(!HasPipe<NoView, decltype(xviews::as_rvalue)>);
static_assert(std::is_same_v<decltype(xviews::as_rvalue),
    decltype(xranges::views::as_rvalue)>);

struct move_iterator_range {
    constexpr std::move_iterator<int*> begin() const { return {}; }
    constexpr std::move_iterator<int*> end() const { return {}; }
};

static_assert(!xranges::view<move_iterator_range>);
static_assert(xranges::range<move_iterator_range>);

constexpr bool test() {
    { // view | views::as_rvalue
        DefaultConstructibleView v{{}, 3};
        std::same_as<
            xranges::as_rvalue_view<DefaultConstructibleView>> decltype(auto)
            view = v | xviews::as_rvalue;
        assert(view.base().i_ == 3);
    }

    { // adaptor | views::as_rvalue
        DefaultConstructibleView v{{}, 3};
        auto const partial =
            std::views::transform(std::identity{}) | xviews::as_rvalue;
        std::same_as<xranges::as_rvalue_view<xranges::transform_view<
            DefaultConstructibleView, std::identity>>> decltype(auto) view =
            partial(v);
        assert(view.base().base().i_ == 3);
    }

    { // views::as_rvalue | adaptor
        DefaultConstructibleView v{{}, 3};
        auto const partial =
            xviews::as_rvalue | std::views::transform(std::identity{});
        std::same_as<xranges::transform_view<
            xranges::as_rvalue_view<DefaultConstructibleView>,
            std::identity>> decltype(auto) view = partial(v);
        assert(view.base().base().i_ == 3);
    }

    { // rvalue-view | views::as_rvalue
        int a[4] = {1, 2, 3, 4};
        xranges::subrange range(rvalue_iterator{a}, rvalue_iterator{a + 4});
        [[maybe_unused]] std::same_as<
            xranges::subrange<rvalue_iterator<int>>> decltype(auto) rval_range =
            range | xviews::as_rvalue;
    }

    { // range | views::as_rvalue
        [[maybe_unused]] std::same_as<xranges::as_rvalue_view<
            std::views::all_t<std::vector<int>>>> decltype(auto) view =
            std::vector<int>{} | xviews::as_rvalue;
    }

    { // rvalue-range | views::as_rvalue
        [[maybe_unused]] std::same_as<
            std::views::all_t<move_iterator_range>> decltype(auto) view =
            move_iterator_range{} | xviews::as_rvalue;
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
