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

// constexpr T* begin() noexcept;
// constexpr const T* begin() const noexcept;

#include "../../static_asserts.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = __RXX ranges;

struct Empty {};
struct BigType {
    char buffer[64] = {10};
};

constexpr bool test() {
    {
        auto sv = xranges::single_view<int>(42);
        assert(*sv.begin() == 42);

        ASSERT_SAME_TYPE(decltype(sv.begin()), int*);
        static_assert(noexcept(sv.begin()));
    }
    {
        auto const sv = xranges::single_view<int>(42);
        assert(*sv.begin() == 42);

        ASSERT_SAME_TYPE(decltype(sv.begin()), int const*);
        static_assert(noexcept(sv.begin()));
    }

    {
        auto sv = xranges::single_view<Empty>(Empty());
        assert(sv.begin() != nullptr);

        ASSERT_SAME_TYPE(decltype(sv.begin()), Empty*);
    }
    {
        auto const sv = xranges::single_view<Empty>(Empty());
        assert(sv.begin() != nullptr);

        ASSERT_SAME_TYPE(decltype(sv.begin()), Empty const*);
    }

    {
        auto sv = xranges::single_view<BigType>(BigType());
        assert(sv.begin()->buffer[0] == 10);

        ASSERT_SAME_TYPE(decltype(sv.begin()), BigType*);
    }
    {
        auto const sv = xranges::single_view<BigType>(BigType());
        assert(sv.begin()->buffer[0] == 10);

        ASSERT_SAME_TYPE(decltype(sv.begin()), BigType const*);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
