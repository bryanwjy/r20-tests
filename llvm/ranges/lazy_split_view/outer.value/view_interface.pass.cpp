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

// class xranges::lazy_split_view::outer-iterator::value_type;

#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <concepts>

using V = ValueTypeForward;
static_assert(xranges::forward_range<V>);
static_assert(xranges::view<V>);

static_assert(std::is_base_of_v<xranges::view_interface<ValueTypeForward>,
    ValueTypeForward>);

constexpr bool test() {
    // empty()
    {
        {
            SplitViewForward v("abc def", " ");
            auto val = *v.begin();
            assert(!val.empty());
        }

        {
            SplitViewForward v;
            auto val = *v.begin();
            assert(val.empty());
        }
    }

    // operator bool()
    {
        {
            SplitViewForward v("abc def", " ");
            auto val = *v.begin();
            assert(val);
        }

        {
            SplitViewForward v;
            auto val = *v.begin();
            assert(!val);
        }
    }

    // front()
    {
        SplitViewForward v("abc def", " ");
        auto val = *v.begin();
        assert(val.front() == 'a');
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
