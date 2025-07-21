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

// constexpr T* end() noexcept;
// constexpr const T* end() const noexcept;

#include "../static_asserts.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct Empty {};
struct BigType {
    char buffer[64] = {10};
};

constexpr bool test() {
    {
        auto sv = xranges::single_view<int>(42);
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), int*);
        static_assert(noexcept(sv.end()));
    }
    {
        auto const sv = xranges::single_view<int>(42);
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), int const*);
        static_assert(noexcept(sv.end()));
    }

    {
        auto sv = xranges::single_view<Empty>(Empty());
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), Empty*);
    }
    {
        auto const sv = xranges::single_view<Empty>(Empty());
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), Empty const*);
    }

    {
        auto sv = xranges::single_view<BigType>(BigType());
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), BigType*);
    }
    {
        auto const sv = xranges::single_view<BigType>(BigType());
        assert(sv.end() == sv.begin() + 1);

        ASSERT_SAME_TYPE(decltype(sv.end()), BigType const*);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
