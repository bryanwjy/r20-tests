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

// friend constexpr bool operator==(const iterator& x, const iterator& y);
//          requires ref-is-glvalue && forward_range<Base> &&
//                   equality_comparable<iterator_t<range_reference_t<Base>>>;

#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
constexpr bool test() {
    int buffer[4][4] = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    {
        xranges::join_view jv(buffer);
        auto iter1 = jv.begin();
        auto iter2 = jv.begin();
        assert(iter1 == iter2);
        iter1++;
        assert(iter1 != iter2);
        iter2++;
        assert(iter1 == iter2);

        assert(jv.begin() == std::as_const(jv).begin());
    }

    {
        // !ref-is-glvalue
        BidiCommonInner inners[2] = {buffer[0], buffer[1]};
        InnerRValue<BidiCommonOuter<BidiCommonInner>> outer{inners};
        xranges::join_view jv(outer);
        auto iter = jv.begin();
        static_assert(!std::equality_comparable<decltype(iter)>);
    }

    {
        // !forward_range<Base>
        using Inner = BufferView<int*>;
        using Outer = BufferView<cpp20_input_iterator<Inner*>,
            sentinel_wrapper<cpp20_input_iterator<Inner*>>>;
        static_assert(!std::equality_comparable<xranges::iterator_t<Outer>>);
        Inner inners[2] = {buffer[0], buffer[1]};
        xranges::join_view jv(Outer{inners});
        auto iter = jv.begin();
        static_assert(!std::equality_comparable<decltype(iter)>);
    }

    {
        // !equality_comparable<iterator_t<range_reference_t<Base>>>;
        using Inner = BufferView<cpp20_input_iterator<int*>,
            sentinel_wrapper<cpp20_input_iterator<int*>>>;
        Inner inners[1] = {buffer[0]};
        xranges::join_view jv{inners};
        auto iter = jv.begin();
        static_assert(!std::equality_comparable<decltype(iter)>);
        auto const_iter = std::as_const(jv).begin();
        static_assert(!std::equality_comparable<decltype(const_iter)>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
