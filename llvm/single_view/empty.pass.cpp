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

// static constexpr bool empty() noexcept;

#include "rxx/ranges/access.h"
#include "rxx/ranges/single_view.h"

#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Empty {};
struct BigType {
    char buffer[64] = {10};
};

template <typename T>
constexpr void test_empty(T value) {
    using SingleView = xranges::single_view<T>;

    {
        std::same_as<bool> decltype(auto) result = SingleView::empty();
        assert(result == false);
        static_assert(noexcept(SingleView::empty()));
    }

    {
        SingleView sv{value};

        std::same_as<bool> decltype(auto) result = xranges::empty(sv);
        assert(result == false);
        static_assert(noexcept(xranges::empty(sv)));
    }
    {
        SingleView const sv{value};

        std::same_as<bool> decltype(auto) result = xranges::empty(sv);
        assert(result == false);
        static_assert(noexcept(xranges::empty(std::as_const(sv))));
    }
}

constexpr bool test() {
    test_empty<int>(92);
    test_empty<Empty>(Empty{});
    test_empty<BigType>(BigType{});

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
