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

// friend constexpr bool operator==(const inner-iterator& x, const
// inner-iterator& y);
//   requires forward_range<Base>;
//
// friend constexpr bool operator==(const inner-iterator& x,
// default_sentinel_t);

#include "../../test_range.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <concepts>
#include <string_view>

constexpr bool test() {
    // When `View` is a forward range, `inner-iterator` supports both overloads
    // of `operator==`.
    {
        SplitViewForward v("abc def", " ");
        auto val = *v.begin();
        auto b = val.begin();
        std::same_as<std::default_sentinel_t> decltype(auto) e = val.end();

        // inner-iterator == inner-iterator
        {
            assert(b == b);
            assert(!(b != b));
        }

        // inner-iterator == default_sentinel
        {
            assert(!(b == e));
            assert(b != e);

            assert(!(b == std::default_sentinel));
            assert(b != std::default_sentinel);
        }
    }

    // When `View` is an input range, `inner-iterator only supports comparing an
    // `inner-iterator` to the default sentinel.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
    if (!std::is_constant_evaluated())
#endif
    {
        SplitViewInput v("abc def", ' ');
        auto val = *v.begin();
        auto b = val.begin();
        std::same_as<std::default_sentinel_t> decltype(auto) e = val.end();

        static_assert(
            !weakly_equality_comparable_with<decltype(b), decltype(b)>);

        assert(!(b == std::default_sentinel));
        assert(b != std::default_sentinel);
        assert(!(b == e));
        assert(b != e);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
