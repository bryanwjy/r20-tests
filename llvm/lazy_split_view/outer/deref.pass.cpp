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

// constexpr outer-iterator::value-type outer-iterator::operator*() const;

#include "../types.h"
#include "rxx/ranges.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>

template <class View, class Separator>
constexpr void test_one(Separator sep) {
    using namespace std::string_literals;
    using namespace std::string_view_literals;
#if RXX_LIBSTDCXX && \
    (!RXX_LIBSTDCXX_AT_LEAST(15) || !RXX_COMPILER_CLANG_AT_LEAST(22, 0, 0))
    // Clang has a language bug related to splitting template declaration
    // and definition: https://github.com/llvm/llvm-project/issues/73232
    // Tentatively assume it will be fixed by llvm-22
    // `View` is a forward range.
    if (std::is_constant_evaluated())
        return;
#endif

    View v("abc def ghi"sv, sep);

    // Non-const iterator.
    {
        auto i = v.begin();
        static_assert(!std::is_reference_v<decltype(*i)>);
        assert(xranges::equal(*i, "abc"s));
        assert(xranges::equal(*(++i), "def"s));
        assert(xranges::equal(*(++i), "ghi"s));
    }

    // Const iterator.
    {
        auto const ci = v.begin();
        static_assert(!std::is_reference_v<decltype(*ci)>);
        assert(xranges::equal(*ci, "abc"s));
    }
}

constexpr bool test() {
    test_one<SplitViewDiff>(" ");
    test_one<SplitViewInput>(' ');

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
