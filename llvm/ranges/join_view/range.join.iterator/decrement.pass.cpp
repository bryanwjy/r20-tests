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

// constexpr iterator& operator--();
//              requires ref-is-glvalue && bidirectional_range<Base> &&
//                       bidirectional_range<range_reference_t<Base>> &&
//                       common_range<range_reference_t<Base>>;
// constexpr iterator operator--(int);
//              requires ref-is-glvalue && bidirectional_range<Base> &&
//                       bidirectional_range<range_reference_t<Base>> &&
//                       common_range<range_reference_t<Base>>;

#include "../types.h"
#include "rxx/algorithm.h"
#include "rxx/ranges.h"
#include "rxx/ranges/transform_view.h"

#include <array>
#include <cassert>
#include <ranges>
#include <type_traits>
#include <vector>

namespace xranges = __RXX ranges;
namespace xviews = __RXX views;
template <class T>
concept CanPreDecrement = requires(T& t) { --t; };

template <class T>
concept CanPostDecrement = requires(T& t) { t--; };

constexpr void noDecrementTest(auto&& jv) {
    auto iter = jv.begin();
    static_assert(!CanPreDecrement<decltype(iter)>);
    static_assert(!CanPostDecrement<decltype(iter)>);
}

constexpr bool test() {
    int buffer[4][4] = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    {
        // outer == ranges::end
        xranges::join_view jv(buffer);
        auto iter = std::next(jv.begin(), 16);
        for (int i = 16; i != 0; --i) {
            assert(*--iter == i);
        }
    }

    {
        // outer == ranges::end
        xranges::join_view jv(buffer);
        auto iter = std::next(jv.begin(), 13);
        for (int i = 13; i != 0; --i) {
            assert(*--iter == i);
        }
    }

    {
        // outer != ranges::end
        xranges::join_view jv(buffer);
        auto iter = std::next(jv.begin(), 12);
        for (int i = 12; i != 0; --i) {
            assert(*--iter == i);
        }
    }

    {
        // outer != ranges::end
        xranges::join_view jv(buffer);
        auto iter = std::next(jv.begin());
        for (int i = 1; i != 0; --i) {
            assert(*--iter == i);
        }
    }

    {
        int small[2][1] = {{1}, {2}};
        xranges::join_view jv(small);
        auto iter = std::next(jv.begin(), 2);
        for (int i = 2; i != 0; --i) {
            assert(*--iter == i);
        }
    }

    {
#if defined(__GNUG__) && !defined(__clang__)
        // This seems to be a gcc bug where evaluating the following code
        // at compile time results in wrong array index
        if (!std::is_constant_evaluated()) {
#endif
            // skip empty inner
            BidiCommonInner inners[4] = {
                buffer[0], {nullptr, 0},
                 {nullptr, 0},
                 buffer[1]
            };
            xranges::join_view jv(inners);
            auto iter = jv.end();
            for (int i = 8; i != 0; --i) {
                assert(*--iter == i);
            }
#if defined(__GNUG__) && !defined(__clang__)
        }
#endif
    }

    {
        // basic type checking
        xranges::join_view jv(buffer);
        auto iter1 = xranges::next(jv.begin(), 4);
        using iterator = decltype(iter1);

        decltype(auto) iter2 = --iter1;
        static_assert(std::same_as<decltype(iter2), iterator&>);
        assert(&iter1 == &iter2);

        std::same_as<iterator> decltype(auto) iter3 = iter1--;
        assert(iter3 == std::next(iter1));
    }

    {
        // !ref-is-glvalue
        BidiCommonInner inners[2] = {buffer[0], buffer[1]};
        InnerRValue<BidiCommonOuter<BidiCommonInner>> outer{inners};
        xranges::join_view jv(outer);
        noDecrementTest(jv);
    }

    {
        // !bidirectional_range<Base>
        BidiCommonInner inners[2] = {buffer[0], buffer[1]};
        SimpleForwardCommonOuter<BidiCommonInner> outer{inners};
        xranges::join_view jv(outer);
        noDecrementTest(jv);
    }

    {
        // !bidirectional_range<range_reference_t<Base>>
        ForwardCommonInner inners[2] = {buffer[0], buffer[1]};
        xranges::join_view jv(inners);
        noDecrementTest(jv);
    }

    {
        // LWG3313 `join_view::iterator::operator--` is incorrectly constrained
        // `join_view::iterator` should not have `operator--` if
        // !common_range<range_reference_t<Base>>
        BidiNonCommonInner inners[2] = {buffer[0], buffer[1]};
        xranges::join_view jv(inners);
        auto iter = jv.begin();
        static_assert(!CanPreDecrement<decltype(iter)>);
        static_assert(!CanPostDecrement<decltype(iter)>);
    }

    {
        // LWG3791: `join_view::iterator::operator--` may be ill-formed
        std::vector<std::vector<int>> vec = {
            {1, 2},
            {3, 4},
            {5, 6}
        };
        auto r = vec |
            xviews::transform([](auto& x) -> auto&& { return std::move(x); }) |
            xviews::join;
        auto e = --r.end();
        assert(*e == 6);
        assert(
            xranges::equal(xviews::reverse(r), std::array{6, 5, 4, 3, 2, 1}));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
