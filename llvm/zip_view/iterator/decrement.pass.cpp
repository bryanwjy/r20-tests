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

// constexpr iterator& operator--() requires all-bidirectional<Const, Views...>;
// constexpr iterator operator--(int) requires all-bidirectional<Const,
// Views...>;

#include "../types.h"
#include "rxx/ranges.h"

#include <array>
#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class Iter>
concept canDecrement =
    requires(Iter it) { --it; } || requires(Iter it) { it--; };

struct NonBidi : IntBufferView {
    using IntBufferView::IntBufferView;
    using iterator = forward_iterator<int*>;
    constexpr iterator begin() const { return iterator(buffer_); }
    constexpr sentinel_wrapper<iterator> end() const {
        return sentinel_wrapper<iterator>(iterator(buffer_ + size_));
    }
};

constexpr bool test() {
    std::array a{1, 2, 3, 4};
    std::array b{4.1, 3.2, 4.3};
    {
        // all random access
        xranges::zip_view v(a, b, xviews::iota(0, 5));
        auto it = v.end();
        using Iter = decltype(it);

        static_assert(std::is_same_v<decltype(--it), Iter&>);
        auto& it_ref = --it;
        assert(&it_ref == &it);

        assert(&(xranges::get_element<0>(*it)) == &(a[2]));
        assert(&(xranges::get_element<1>(*it)) == &(b[2]));
        assert(xranges::get_element<2>(*it) == 2);

        static_assert(std::is_same_v<decltype(it--), Iter>);
        it--;
        assert(&(xranges::get_element<0>(*it)) == &(a[1]));
        assert(&(xranges::get_element<1>(*it)) == &(b[1]));
        assert(xranges::get_element<2>(*it) == 1);
    }

    {
        // all bidi+
        int buffer[2] = {1, 2};

        xranges::zip_view v(BidiCommonView{buffer}, xviews::iota(0, 5));
        auto it = v.begin();
        using Iter = decltype(it);

        ++it;
        ++it;

        static_assert(std::is_same_v<decltype(--it), Iter&>);
        auto& it_ref = --it;
        assert(&it_ref == &it);

        assert(it == ++v.begin());

        static_assert(std::is_same_v<decltype(it--), Iter>);
        auto tmp = it--;
        assert(it == v.begin());
        assert(tmp == ++v.begin());
    }

    {
        // non bidi
        int buffer[3] = {4, 5, 6};
        xranges::zip_view v(a, NonBidi{buffer});
        using Iter = xranges::iterator_t<decltype(v)>;
        static_assert(!canDecrement<Iter>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
