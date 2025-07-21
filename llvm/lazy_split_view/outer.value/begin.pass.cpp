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

// rxx::ranges::lazy_split_view::outer-iterator::value_type::begin()

#include "../types.h"
#include "rxx/ranges/lazy_split_view.h"

#include <cassert>

constexpr bool test() {
    // `View` is a forward range.
    {
        CopyableView input("a");

        // Non-const.
        {
            SplitViewCopyable v(input, "b");
            auto val = *v.begin();
            assert(val.begin().base() == input.begin());
        }

        // Const.
        {
            SplitViewCopyable v(input, "b");
            auto const val = *v.begin();
            assert(val.begin().base() == input.begin());
        }
    }

    // `View` is an input range.
    {
        InputView input("a");

        // Non-const.
        {
            SplitViewInput v(input, 'b');
            auto val = *v.begin();
            // Copies of `InputView` are independent and the iterators won't
            // compare the same.
            assert(*val.begin().base() == *input.begin());
        }

        // Const.
        {
            SplitViewInput v(input, 'b');
            auto const val = *v.begin();
            // Copies of `InputView` are independent and the iterators won't
            // compare the same.
            assert(*val.begin().base() == *input.begin());
        }
    }

    return true;
}

int main(int, char**) {
    assert(test());
    static_assert(test());

    return 0;
}
