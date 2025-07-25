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

// constexpr auto begin();
// constexpr auto begin() const requires forward_range<View> &&
// forward_range<const View>;

#include "../test_iterators.h"
#include "../test_range.h"
#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <utility>

template <class View>
concept ConstBeginDisabled = !requires(View const v) {
    { (*v.begin()) };
};

constexpr bool test() {
    // non-const: forward_range<View> && simple-view<View> ->
    // outer-iterator<Const = true> const: forward_range<View> &&
    // forward_range<const View> -> outer-iterator<Const = true>
    {
        using V = ForwardView;
        using P = V;

        static_assert(xranges::forward_range<V>);
        static_assert(xranges::forward_range<V const>);
        static_assert(simple_view<V>);
        static_assert(simple_view<P>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }
    }

    // non-const: forward_range<View> && !simple-view<View> ->
    // outer-iterator<Const = false> const: forward_range<View> &&
    // forward_range<const View> -> outer-iterator<Const = true>
    {
        using V = ForwardDiffView;
        using P = V;

        static_assert(xranges::forward_range<V>);
        static_assert(xranges::forward_range<V const>);
        static_assert(!simple_view<V>);
        static_assert(!simple_view<P>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(std::is_same_v<decltype(*(*it).begin()), char&>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }
    }

    // non-const: forward_range<View> && !simple-view<View> ->
    // outer-iterator<Const = false> const: forward_range<View> &&
    // !forward_range<const View> -> disabled
    {
        using V = ForwardOnlyIfNonConstView;
        using P = V;
        static_assert(xranges::forward_range<V>);
        static_assert(!xranges::forward_range<V const>);
        static_assert(!simple_view<V>);
        static_assert(!simple_view<P>);

        xranges::lazy_split_view<V, P> v;
        auto it = v.begin();
        static_assert(std::is_same_v<decltype(it)::iterator_concept,
            std::forward_iterator_tag>);
        static_assert(std::is_same_v<decltype(*(*it).begin()), char const&>);

        static_assert(ConstBeginDisabled<decltype(v)>);
    }

    // non-const: forward_range<View> && simple-view<View> &&
    // !simple-view<Pattern> -> outer-iterator<Const = false> const:
    // forward_range<View> && forward_range<const View> -> outer-iterator<Const
    // = true>
    {
        using V = ForwardView;
        using P = ForwardOnlyIfNonConstView;

        static_assert(xranges::forward_range<V>);
        static_assert(xranges::forward_range<V const>);
        static_assert(simple_view<V>);
        static_assert(!simple_view<P>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.begin();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }
    }

    // non-const: !forward_range<View> && tiny-range<Pattern> ->
    // outer-iterator<Const = false> const: !forward_range<View> -> disabled
    {
        using V = InputView;
        using P = ForwardTinyView;

        static_assert(!xranges::forward_range<V>);
        static_assert(xranges::forward_range<P>);

        xranges::lazy_split_view<V, P> v;
        auto it = v.begin();
        static_assert(std::is_same_v<decltype(it)::iterator_concept,
            std::input_iterator_tag>);
        static_assert(std::is_same_v<decltype(*(*it).begin()), char&>);

        static_assert(ConstBeginDisabled<decltype(v)>);
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
