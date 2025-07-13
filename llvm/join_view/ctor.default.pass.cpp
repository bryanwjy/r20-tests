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

// join_view() requires default_initializable<V> = default;

#include "rxx/join_view.h"
#include "types.h"

#include <cassert>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
struct DefaultView : std::ranges::view_base {
    int i; // deliberately uninitialised

    ChildView* begin() const;
    ChildView* end() const;
};

constexpr bool test() {
    {
        xranges::join_view<ParentView<ChildView>> jv;
        assert(std::move(jv).base().ptr_ == globalChildren);
    }

    // Default constructor should value initialise underlying view
    {
        xranges::join_view<DefaultView> jv;
        assert(jv.base().i == 0);
    }

    static_assert(
        std::default_initializable<xranges::join_view<ParentView<ChildView>>>);
    static_assert(
        !std::default_initializable<xranges::join_view<CopyableParent>>);

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
