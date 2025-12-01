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

// constexpr decltype(auto) inner-iterator::operator*() const;

#include "../types.h"
#include "rxx/ranges.h"

constexpr bool test() {
    // Can call `inner-iterator::operator*`; `View` is a forward range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (!std::is_constant_evaluated())
#endif
    {
        SplitViewDiff v("abc def", " ");
        auto val = *v.begin();

        // Non-const iterator.
        {
            auto i = val.begin();
            static_assert(std::same_as<decltype(*i), char&>);
            assert(*i == 'a');
            assert(*(++i) == 'b');
            assert(*(++i) == 'c');
        }

        // Const iterator.
        {
            auto const ci = val.begin();
            static_assert(std::same_as<decltype(*ci), char&>);
            assert(*ci == 'a');
        }
    }

    // Can call `inner-iterator::operator*`; `View` is an input range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (!std::is_constant_evaluated())
#endif
    {
        SplitViewInput v("abc def", ' ');
        auto val = *v.begin();

        // Non-const iterator.
        {
            auto i = val.begin();
            static_assert(std::same_as<decltype(*i), char&>);
            assert(*i == 'a');
            assert(*(++i) == 'b');
            assert(*(++i) == 'c');
        }

        // Const iterator.
        {
            auto const ci = val.begin();
            static_assert(std::same_as<decltype(*ci), char&>);
            // Note: when the underlying range is an input range, `current` is
            // stored in the `lazy_split_view` itself and shared between
            // `inner-iterator`s. Consequently, incrementing one iterator
            // effectively increments all of them.
            assert(*ci == 'c');
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
