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

// iterator() = default;

#include "../types.h"
#include "rxx/ranges.h"

namespace xranges = __RXX ranges;

struct PODIter {
    int i; // deliberately uninitialised

    using iterator_category = std::random_access_iterator_tag;
    using value_type = int;
    using difference_type = std::intptr_t;

    constexpr int operator*() const { return i; }

    constexpr PODIter& operator++() { return *this; }
    constexpr void operator++(int) {}

    friend constexpr bool operator==(PODIter const&, PODIter const&) = default;
};

struct IterDefaultCtrView : xranges::view_base {
    PODIter begin() const;
    PODIter end() const;
};

struct IterNoDefaultCtrView : xranges::view_base {
    cpp20_input_iterator<int*> begin() const;
    sentinel_wrapper<cpp20_input_iterator<int*>> end() const;
};

template <class... Views>
using zip_iter = xranges::iterator_t<xranges::zip_view<Views...>>;

static_assert(!std::default_initializable<zip_iter<IterNoDefaultCtrView>>);
static_assert(!std::default_initializable<
    zip_iter<IterNoDefaultCtrView, IterDefaultCtrView>>);
static_assert(!std::default_initializable<
    zip_iter<IterNoDefaultCtrView, IterNoDefaultCtrView>>);
static_assert(std::default_initializable<zip_iter<IterDefaultCtrView>>);
static_assert(std::default_initializable<
    zip_iter<IterDefaultCtrView, IterDefaultCtrView>>);

constexpr bool test() {
    using ZipIter = zip_iter<IterDefaultCtrView>;
    {
        ZipIter iter;
        auto [x] = *iter;
        assert(x == 0); // PODIter has to be initialised to have value 0
    }

    {
        ZipIter iter = {};
        auto [x] = *iter;
        assert(x == 0); // PODIter has to be initialised to have value 0
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
