// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// sentinel() = default;

#include "rxx/ranges.h"

#include <cassert>
#include <tuple>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

struct PODSentinel {
    bool b; // deliberately uninitialised

    friend constexpr bool operator==(int*, PODSentinel const& s) { return s.b; }
};

struct Range : xranges::view_base {
    int* begin() const;
    PODSentinel end();
};

constexpr bool test() {
    {
        using R = xranges::zip_view<Range>;
        using Sentinel = xranges::sentinel_t<R>;
        static_assert(!std::is_same_v<Sentinel, xranges::iterator_t<R>>);

        xranges::iterator_t<R> it;

        Sentinel s1;
        assert(it != s1); // PODSentinel.b is initialised to false

        Sentinel s2 = {};
        assert(it != s2); // PODSentinel.b is initialised to false
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
