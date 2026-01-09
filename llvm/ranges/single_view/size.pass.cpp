// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// static constexpr size_t size() noexcept;

#include "../../static_asserts.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = __RXX ranges;

constexpr bool test() {
    {
        auto sv = xranges::single_view<int>(42);
        assert(sv.size() == 1);

        ASSERT_SAME_TYPE(decltype(sv.size()), std::size_t);
        static_assert(noexcept(sv.size()));
    }
    {
        auto const sv = xranges::single_view<int>(42);
        assert(sv.size() == 1);

        ASSERT_SAME_TYPE(decltype(sv.size()), std::size_t);
        static_assert(noexcept(sv.size()));
    }
    {
        auto sv = xranges::single_view<int>(42);
        assert(xranges::size(sv) == 1);

        ASSERT_SAME_TYPE(decltype(xranges::size(sv)), std::size_t);
        static_assert(noexcept(xranges::size(sv)));
    }
    {
        auto const sv = xranges::single_view<int>(42);
        assert(xranges::size(sv) == 1);

        ASSERT_SAME_TYPE(decltype(xranges::size(sv)), std::size_t);
        static_assert(noexcept(xranges::size(sv)));
    }

    // Test that it's static.
    {
        assert(xranges::single_view<int>::size() == 1);

        ASSERT_SAME_TYPE(
            decltype(xranges::single_view<int>::size()), std::size_t);
        static_assert(noexcept(xranges::single_view<int>::size()));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
