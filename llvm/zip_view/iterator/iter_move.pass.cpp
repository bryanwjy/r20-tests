// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// friend constexpr auto iter_move(const iterator& i) noexcept(see below);

#include "../types.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <iterator>
#include <tuple>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct ThrowingMove {
    ThrowingMove() = default;
    ThrowingMove(ThrowingMove&&) {}
};

constexpr bool test() {
    {
        // underlying iter_move noexcept
        std::array a1{1, 2, 3, 4};
        std::array const a2{3.0, 4.0};

        xranges::zip_view v(a1, a2, xviews::iota(3L));
        assert(xranges::iter_move(v.begin()) == std::make_tuple(1, 3.0, 3L));
        static_assert(std::is_same_v<decltype(xranges::iter_move(v.begin())),
            rxx::tuple<int&&, double const&&, long>>);
        static_assert(
            std::is_convertible_v<decltype(xranges::iter_move(v.begin())),
                std::tuple<int&&, double const&&, long>>);

        auto it = v.begin();
        static_assert(noexcept(xranges::iter_move(it)));
    }

    {
        // underlying iter_move may throw
        auto throwingMoveRange = xviews::iota(0, 2) |
            xviews::transform([](auto) noexcept { return ThrowingMove{}; });
        xranges::zip_view v(throwingMoveRange);
        auto it = v.begin();
        static_assert(!noexcept(xranges::iter_move(it)));
    }

    {
        // underlying iterators' iter_move are called through ranges::iter_move
        adltest::IterMoveSwapRange r1{}, r2{};
        assert(r1.iter_move_called_times == 0);
        assert(r2.iter_move_called_times == 0);
        xranges::zip_view v(r1, r2);
        auto it = v.begin();
        {
            [[maybe_unused]] auto&& i = xranges::iter_move(it);
            assert(r1.iter_move_called_times == 1);
            assert(r2.iter_move_called_times == 1);
        }
        {
            [[maybe_unused]] auto&& i = xranges::iter_move(it);
            assert(r1.iter_move_called_times == 2);
            assert(r2.iter_move_called_times == 2);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
