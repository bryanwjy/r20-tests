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

// constexpr auto end() requires forward_range<View> && common_range<View>;
// constexpr auto end() const;

#include "../test_iterators.h"
#include "../test_range.h"
#include "rxx/ranges.h"
#include "types.h"

#include <cassert>
#include <utility>

struct ForwardViewCommonIfConst : xranges::view_base {
    std::string_view view_;
    constexpr explicit ForwardViewCommonIfConst() = default;
    constexpr ForwardViewCommonIfConst(char const* ptr) : view_(ptr) {}
    constexpr ForwardViewCommonIfConst(std::string_view v) : view_(v) {}
    constexpr ForwardViewCommonIfConst(ForwardViewCommonIfConst&&) = default;
    constexpr ForwardViewCommonIfConst& operator=(
        ForwardViewCommonIfConst&&) = default;
    constexpr ForwardViewCommonIfConst(
        ForwardViewCommonIfConst const&) = default;
    constexpr ForwardViewCommonIfConst& operator=(
        ForwardViewCommonIfConst const&) = default;
    constexpr forward_iterator<char*> begin() {
        return forward_iterator<char*>(nullptr);
    }
    constexpr std::default_sentinel_t end() { return std::default_sentinel; }
    constexpr forward_iterator<std::string_view::const_iterator> begin() const {
        return forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr forward_iterator<std::string_view::const_iterator> end() const {
        return forward_iterator<std::string_view::const_iterator>(view_.end());
    }
};
bool operator==(forward_iterator<char*>, std::default_sentinel_t) {
    return false;
}

struct ForwardViewNonCommonRange : xranges::view_base {
    std::string_view view_;
    constexpr explicit ForwardViewNonCommonRange() = default;
    constexpr ForwardViewNonCommonRange(char const* ptr) : view_(ptr) {}
    constexpr ForwardViewNonCommonRange(std::string_view v) : view_(v) {}
    constexpr ForwardViewNonCommonRange(ForwardViewNonCommonRange&&) = default;
    constexpr ForwardViewNonCommonRange& operator=(
        ForwardViewNonCommonRange&&) = default;
    constexpr ForwardViewNonCommonRange(
        ForwardViewNonCommonRange const&) = default;
    constexpr ForwardViewNonCommonRange& operator=(
        ForwardViewNonCommonRange const&) = default;
    constexpr forward_iterator<char*> begin() {
        return forward_iterator<char*>(nullptr);
    }
    constexpr std::default_sentinel_t end() { return std::default_sentinel; }
    constexpr forward_iterator<std::string_view::const_iterator> begin() const {
        return forward_iterator<std::string_view::const_iterator>(
            view_.begin());
    }
    constexpr std::default_sentinel_t end() const {
        return std::default_sentinel;
    }
};
bool operator==(forward_iterator<std::string_view::const_iterator>,
    std::default_sentinel_t) {
    return false;
}

constexpr bool test() {
    // non-const: forward_range<V> && simple_view<V> && simple_view<P> ->
    // outer-iterator<Const = true> const: forward_range<V> && common_range<V>
    // -> outer-iterator<Const = true>
    {
        using V = ForwardView;
        using P = V;

        static_assert(xranges::forward_range<V>);
        static_assert(xranges::common_range<V const>);
        static_assert(simple_view<V>);
        static_assert(simple_view<P>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.end();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.end();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }
    }

    // non-const: forward_range<V> && common_range<V> && simple_view<V> &&
    // !simple_view<P> -> outer-iterator<Const=false> const: forward_range<V> &&
    // forward_range<const V> && common_range<const V> -> outer-iterator<Const =
    // false>
    {
        using V = ForwardView;
        using P = ForwardDiffView;

        static_assert(xranges::forward_range<V>);
        static_assert(xranges::common_range<V>);
        static_assert(simple_view<V>);
        static_assert(!simple_view<P>);
        static_assert(xranges::forward_range<V const>);
        static_assert(xranges::common_range<V const>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.end();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.end();
            static_assert(std::is_same_v<decltype(it)::iterator_concept,
                std::forward_iterator_tag>);
            static_assert(
                std::is_same_v<decltype(*(*it).begin()), char const&>);
        }
    }

    // non-const: forward_range<V> && !common_range<V> -> disabled
    // const: forward_range<V> && forward_range<const V> && common_range<const
    // V> -> outer-iterator<Const = true>
    {
        using V = ForwardViewCommonIfConst;
        using P = V;

        static_assert(xranges::forward_range<V>);
        static_assert(!xranges::common_range<V>);
        static_assert(xranges::forward_range<V const>);
        static_assert(xranges::common_range<V const>);

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

    // non-const: forward_range<V> && !common_range<V> -> disabled
    // const: forward_range<V> && forward_range<const V> && !common_range<const
    // V> -> outer-iterator<Const = false>
    {
        using V = ForwardViewNonCommonRange;
        using P = V;

        static_assert(xranges::forward_range<V>);
        static_assert(!xranges::common_range<V>);
        static_assert(xranges::forward_range<V const>);
        static_assert(!xranges::common_range<V const>);

        {
            xranges::lazy_split_view<V, P> v;
            auto it = v.end();
            static_assert(std::same_as<decltype(it), std::default_sentinel_t>);
        }

        {
            xranges::lazy_split_view<V, P> const cv;
            auto it = cv.end();
            static_assert(std::same_as<decltype(it), std::default_sentinel_t>);
        }
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
