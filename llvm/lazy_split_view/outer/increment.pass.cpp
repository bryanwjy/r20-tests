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

// constexpr outer-iterator& outer-iterator::operator++();
// constexpr decltype(auto) outer-iterator::operator++(int);

// Note that corner cases are tested in `range.lazy.split/general.pass.cpp`.

#include "../types.h"
#include "rxx/ranges.h"

#include <algorithm>
#include <cassert>
#include <string>

constexpr bool test() {
#if RXX_LIBSTDCXX && \
    (!RXX_LIBSTDCXX_AT_LEAST(15) || !RXX_COMPILER_CLANG_AT_LEAST(22, 0, 0))
    // Clang has a language bug related to splitting template declaration
    // and definition taht libstdc++ does. See
    // https://github.com/llvm/llvm-project/issues/73232.
    // Tentatively assume it will be fixed by llvm-22
    if (std::is_constant_evaluated())
        return true;
#endif

    using namespace std::string_literals;
    // Can call `outer-iterator::operator++`; `View` is a forward range.
    {
        SplitViewForward v("abc def ghi", " ");

        // ++i
        {
            auto i = v.begin();
            assert(xranges::equal(*i, "abc"s));

            decltype(auto) i2 = ++i;
            static_assert(std::is_lvalue_reference_v<decltype(i2)>);
            assert(&i2 == &i);
            assert(xranges::equal(*i2, "def"s));
        }

        // i++
        {
            auto i = v.begin();
            assert(xranges::equal(*i, "abc"s));

            decltype(auto) i2 = i++;
            static_assert(!std::is_reference_v<decltype(i2)>);
            assert(xranges::equal(*i2, "abc"s));
            assert(xranges::equal(*i, "def"s));
        }
    }

    // Can call `outer-iterator::operator++`; `View` is an input range.
    {
        SplitViewInput v("abc def ghi", ' ');

        // ++i
        {
            auto i = v.begin();
            assert(xranges::equal(*i, "abc"s));

            decltype(auto) i2 = ++i;
            static_assert(std::is_lvalue_reference_v<decltype(i2)>);
            assert(&i2 == &i);
            assert(xranges::equal(*i2, "def"s));
        }

        // i++
        {
            auto i = v.begin();
            assert(xranges::equal(*i, "abc"s));

            static_assert(std::is_void_v<decltype(i++)>);
            i++;
            assert(xranges::equal(*i, "def"s));
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
