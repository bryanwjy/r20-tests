// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../MoveOnly.h"
#include "rxx/ranges/as_rvalue_view.h"
#include "rxx/ranges/view_base.h"

#include <cassert>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct SimpleView : xranges::view_base {
    int i;
    int* begin() const;
    int* end() const;
};

struct MoveOnlyView : SimpleView {
    MoveOnly m;
};

template <class T>
concept HasBase = requires(T&& t) { std::forward<T>(t).base(); };

static_assert(HasBase<xranges::as_rvalue_view<SimpleView> const&>);
static_assert(HasBase<xranges::as_rvalue_view<SimpleView>&&>);

static_assert(!HasBase<xranges::as_rvalue_view<MoveOnlyView> const&>);
static_assert(HasBase<xranges::as_rvalue_view<MoveOnlyView>&&>);

constexpr bool test() {
    { // const &
        xranges::as_rvalue_view<SimpleView> const view(SimpleView{{}, 5});
        std::same_as<SimpleView> decltype(auto) v = view.base();
        assert(v.i == 5);
    }

    { // &
        xranges::as_rvalue_view<SimpleView> view(SimpleView{{}, 5});
        std::same_as<SimpleView> decltype(auto) v = view.base();
        assert(v.i == 5);
    }

    { // &&
        xranges::as_rvalue_view<SimpleView> view(SimpleView{{}, 5});
        std::same_as<SimpleView> decltype(auto) v = std::move(view).base();
        assert(v.i == 5);
    }

    { // const &&
        xranges::as_rvalue_view<SimpleView> const view(SimpleView{{}, 5});
        std::same_as<SimpleView> decltype(auto) v = std::move(view).base();
        assert(v.i == 5);
    }

    { // move only
        xranges::as_rvalue_view<MoveOnlyView> view(MoveOnlyView{{}, 5});
        std::same_as<MoveOnlyView> decltype(auto) v = std::move(view).base();
        assert(v.m.get() == 5);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
