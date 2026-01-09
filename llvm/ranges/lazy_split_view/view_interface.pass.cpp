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

// class xranges::lazy_split_view;

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <concepts>
#include <string_view>
#include <type_traits>

using V = SplitViewForward;

static_assert(std::is_base_of_v<xranges::view_interface<SplitViewForward>,
    SplitViewForward>);

constexpr bool test() {
    using namespace std::string_view_literals;

    // empty()
    {
        {
            xranges::lazy_split_view v("abc def", " ");
            assert(!v.empty());
        }

        {
            // Note: an empty string literal would still produce a non-empty
            // output because the terminating zero is treated as a separate
            // character; hence the use of `string_view`.
            xranges::lazy_split_view v(""sv, "");
            assert(v.empty());
        }
    }

    // operator bool()
    {
        {
            xranges::lazy_split_view v("abc", "");
            assert(v);
        }

        {
            // Note: an empty string literal would still produce a non-empty
            // output because the terminating zero is treated as a separate
            // character; hence the use of `string_view`.
            xranges::lazy_split_view v(""sv, "");
            assert(!v);
        }
    }

    // front()
    {
        SplitViewForward v("abc", "");
        assert(*(v.front()).begin() == 'a');
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
