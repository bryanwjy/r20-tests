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

// constexpr explicit single_view(const T& t);
// constexpr explicit single_view(T&& t);

#include "rxx/single_view.h"

#include <cassert>
#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Empty {};
struct BigType {
    char buffer[64] = {10};
};

constexpr bool test() {
    {
        BigType bt;
        xranges::single_view<BigType> sv(bt);
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }
    {
        BigType const bt;
        xranges::single_view<BigType> const sv(bt);
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }

    {
        BigType bt;
        xranges::single_view<BigType> sv(std::move(bt));
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }
    {
        BigType const bt;
        xranges::single_view<BigType> const sv(std::move(bt));
        assert(sv.data()->buffer[0] == 10);
        assert(sv.size() == 1);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
