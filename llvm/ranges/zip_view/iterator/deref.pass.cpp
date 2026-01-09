// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// constexpr auto operator*() const;

#include "../types.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>
#include <tuple>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;

constexpr bool test() {
    std::array a{1, 2, 3, 4};
    std::array b{4.1, 3.2, 4.3};
    {
        // single range
        xranges::zip_view v(a);
        auto it = v.begin();
        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
        static_assert(std::is_same_v<decltype(*it), __RXX tuple<int&>>);
        static_assert(std::is_convertible_v<decltype(*it), __RXX tuple<int&>>);
    }

    {
        // operator* is const
        xranges::zip_view v(a);
        auto const it = v.begin();
        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
    }

    {
        // two ranges with different types
        xranges::zip_view v(a, b);
        auto it = v.begin();
        auto [x, y] = *it;
        assert(&x == &(a[0]));
        assert(&y == &(b[0]));
        static_assert(
            std::is_same_v<decltype(*it), __RXX tuple<int&, double&>>);
        static_assert(
            std::is_convertible_v<decltype(*it), __RXX tuple<int&, double&>>);

        x = 5;
        y = 0.1;
        assert(a[0] == 5);
        assert(b[0] == 0.1);
    }

    {
        // underlying range with prvalue range_reference_t
        xranges::zip_view v(a, b, xviews::iota(0, 5));
        auto it = v.begin();
        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
        assert(&(xranges::get_element<1>(*it)) == &(b[0]));
        assert(xranges::get_element<2>(*it) == 0);
        static_assert(
            std::is_same_v<decltype(*it), __RXX tuple<int&, double&, int>>);
        static_assert(std::is_convertible_v<decltype(*it),
            std::tuple<int&, double&, int>>);
    }

    {
        // const-correctness
        xranges::zip_view v(a, std::as_const(a));
        auto it = v.begin();
        assert(&(xranges::get_element<0>(*it)) == &(a[0]));
        assert(&(xranges::get_element<1>(*it)) == &(a[0]));
        static_assert(
            std::is_same_v<decltype(*it), __RXX tuple<int&, int const&>>);
        static_assert(
            std::is_convertible_v<decltype(*it), std::tuple<int&, int const&>>);
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
