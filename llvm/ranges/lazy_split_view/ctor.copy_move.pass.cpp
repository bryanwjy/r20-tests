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

// Test the implicitly-generated copy and move constructors since
// `lazy_split_view` has non-trivial members.

#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <string_view>
#include <utility>

constexpr bool test() {
    // Can copy `lazy_split_view`.
    {
        // Forward range.
        {
            xranges::lazy_split_view<std::string_view, std::string_view> v1(
                "abc def", " ");
            auto v2 = v1;
            assert(v2.base() == v1.base());
        }

        // Input range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
        if (!std::is_constant_evaluated())
#endif
        {
            SplitViewInput v1("abc def", ' ');
            auto v2 = v1;
            assert(v2.base() == v1.base());
        }
    }

    // Can move `lazy_split_view`.
    {
        // Forward range.
        {
            std::string_view base = "abc def";
            xranges::lazy_split_view<std::string_view, std::string_view> v1(
                base, " ");
            auto v2 = std::move(v1);
            assert(v2.base() == base);
        }

        // Input range.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
        if (!std::is_constant_evaluated())
#endif
        {
            InputView base("abc def");
            SplitViewInput v1(base, ' ');
            auto v2 = std::move(v1);
            assert(v2.base() == base);
        }
    }

    // `non-propagating-cache` is not copied.
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AT_LEAST(15)
    if (!std::is_constant_evaluated())
#endif
    {
        SplitViewInput v1("abc def ghi", ' ');
        auto outer_iter1 = v1.begin();
        ++outer_iter1;
        auto val1 = *outer_iter1;
        auto i1 = val1.begin();
        assert(*i1 == 'd');
        ++i1;
        assert(*i1 == 'e');

        auto v2 = v1;
        auto val2 = *v2.begin();
        auto i2 = val2.begin();
        assert(*i2 == 'a');
        ++i2;
        assert(*i2 == 'b');
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
