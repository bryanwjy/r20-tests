// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// <ranges>

// constexpr iterator(iterator<!Const> i)
//   requires Const && convertible_to<iterator_t<V>, OuterIter> &&
//            convertible_to<iterator_t<InnerRng>, InnerIter> &&
//            convertible_to<iterator_t<Pattern>, PatternIter>;

#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr bool test() {
    { // Regular conversion from `!Const` to `Const` iterator
        std::vector<std::vector<int>> vec = {
            {1, 2},
            {3, 4},
            {5, 6}
        };
        int pattern = 0;
        xranges::join_with_view jwv(vec, pattern);

        using JWV = decltype(jwv);
        using Iter = xranges::iterator_t<JWV>;
        using CIter = xranges::iterator_t<const JWV>;
        static_assert(!std::same_as<Iter, CIter>);
        static_assert(std::convertible_to<Iter, CIter>);
        static_assert(std::constructible_from<CIter, Iter>);

        Iter it = jwv.begin();
        assert(*it == 1);

        CIter const cit1 = it; // `cit1` points to element of `V`; this
                               // constructor should not be explicit
        assert(*cit1 == 1);
        assert(cit1 == it);

        xranges::advance(it, 2);
        assert(*it == 0);
        CIter cit2 = it; // `cit2` points to element of `Pattern`
        assert(*cit2 == 0);
        assert(cit2 == it);

        ++it;
        assert(*it == 3);
        CIter cit3 = it;
        assert(*cit3 == 3);
        assert(cit3 == it);

        --cit3;
        assert(cit2 == cit3);
    }

    { // Test conversion from `Const` to `!Const` (should be invalid)
        using V = std::vector<std::vector<int>>;
        using Pattern = xranges::single_view<int>;
        using JWV = xranges::join_with_view<xviews::all_t<V>, Pattern>;
        using Iter = xranges::iterator_t<JWV>;
        using CIter = xranges::iterator_t<const JWV>;
        static_assert(!std::convertible_to<CIter, Iter>);
        static_assert(!std::constructible_from<Iter, CIter>);
    }

    { // When `convertible_to<iterator_t<V>, OuterIter>` is not modeled
        using Inner = std::vector<short>;
        using V = ConstOppositeView<Inner>;
        using Pattern = xranges::single_view<short>;
        using JWV = xranges::join_with_view<V, Pattern>;
        using Iter = xranges::iterator_t<JWV>;
        using CIter = xranges::iterator_t<const JWV>;
        static_assert(!std::convertible_to<CIter, Iter>);
        static_assert(!std::constructible_from<Iter, CIter>);
    }

    { // When `convertible_to<iterator_t<InnerRng>, InnerIter>` is not modeled
        using Inner = ConstOppositeView<long>;
        using V = std::vector<Inner>;
        using Pattern = xranges::single_view<long>;
        using JWV = xranges::join_with_view<xviews::all_t<V>, Pattern>;
        using Iter = xranges::iterator_t<JWV>;
        using CIter = xranges::iterator_t<const JWV>;
        static_assert(!std::convertible_to<CIter, Iter>);
        static_assert(!std::constructible_from<Iter, CIter>);
    }

    { // When `convertible_to<iterator_t<Pattern>, PatternIter>` is not modeled
        using V = std::vector<std::vector<long long>>;
        using Pattern = ConstOppositeView<long long>;
        using JWV = xranges::join_with_view<xviews::all_t<V>, Pattern>;
        using Iter = xranges::iterator_t<JWV>;
        using CIter = xranges::iterator_t<const JWV>;
        static_assert(!std::convertible_to<CIter, Iter>);
        static_assert(!std::constructible_from<Iter, CIter>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
