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
// constexpr auto begin() const
//    requires forward_range<const V> &&
//             is_reference_v<range_reference_t<const V>> &&
//             input_range<range_reference_t<const V>>;

#include "rxx/ranges.h"
#include "types.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <string_view>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
struct NonSimpleParentView : xranges::view_base {
    ChildView* begin() { return nullptr; }
    ChildView const* begin() const;
    ChildView const* end() const;
};

struct SimpleParentView : xranges::view_base {
    ChildView const* begin() const;
    ChildView const* end() const;
};

struct ConstNotRange : xranges::view_base {
    ChildView const* begin();
    ChildView const* end();
};
static_assert(xranges::range<ConstNotRange>);
static_assert(!xranges::range<ConstNotRange const>);

template <class T>
concept HasConstBegin = requires(T const& t) { t.begin(); };

constexpr bool test() {
    int buffer[4][4] = {
        {1111, 2222, 3333, 4444},
        { 555,  666,  777,  888},
        {  99, 1010, 1111, 1212},
        {  13,   14,   15,   16}
    };

    {
        ChildView children[4] = {ChildView(buffer[0]), ChildView(buffer[1]),
            ChildView(buffer[2]), ChildView(buffer[3])};
        auto jv = xranges::join_view(ParentView{children});
        assert(*jv.begin() == 1111);
    }

    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 1),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView{children});
        assert(*jv.begin() == 1111);
    }

    // Parent is empty.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0]),
            CopyableChild(buffer[1]), CopyableChild(buffer[2]),
            CopyableChild(buffer[3])};
        xranges::join_view jv(ParentView(children, 0));
        assert(jv.begin() == jv.end());
    }

    // Parent size is one.
    {
        CopyableChild children[1] = {CopyableChild(buffer[0])};
        xranges::join_view jv(ParentView(children, 1));
        assert(*jv.begin() == 1111);
    }

    // Parent and child size is one.
    {
        CopyableChild children[1] = {CopyableChild(buffer[0], 1)};
        xranges::join_view jv(ParentView(children, 1));
        assert(*jv.begin() == 1111);
    }

    // Parent size is one child is empty
    {
        CopyableChild children[1] = {CopyableChild(buffer[0], 0)};
        xranges::join_view jv(ParentView(children, 1));
        assert(jv.begin() == jv.end());
    }

    // Has all empty children.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 0),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 0),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView{children});
        assert(jv.begin() == jv.end());
    }

    // First child is empty, others are not.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 0), CopyableChild(buffer[2], 0),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView{children});
        assert(*jv.begin() == 1111);
    }

    // Last child is empty, others are not.
    {
        CopyableChild children[4] = {CopyableChild(buffer[0], 4),
            CopyableChild(buffer[1], 4), CopyableChild(buffer[2], 4),
            CopyableChild(buffer[3], 0)};
        auto jv = xranges::join_view(ParentView{children});
        assert(*jv.begin() == 1111);
    }

    {
        xranges::join_view jv(buffer);
        assert(*jv.begin() == 1111);
    }

    {
        xranges::join_view const jv(buffer);
        assert(*jv.begin() == 1111);
        static_assert(HasConstBegin<decltype(jv)>);
    }

    // !forward_range<const V>
    {
        xranges::join_view jv{ConstNotRange{}};
        static_assert(!HasConstBegin<decltype(jv)>);
    }

    // !is_reference_v<range_reference_t<const V>>
    {
        auto innerRValueRange = xviews::iota(0, 5) |
            xviews::transform([](int) { return ChildView{}; });
        static_assert(
            !std::is_reference_v<
                xranges::range_reference_t<const decltype(innerRValueRange)>>);
        xranges::join_view jv{innerRValueRange};
        static_assert(!HasConstBegin<decltype(jv)>);
    }

    // !simple-view<V>
    {
        xranges::join_view<NonSimpleParentView> jv;
        static_assert(!std::same_as<decltype(jv.begin()),
                      decltype(std::as_const(jv).begin())>);
    }

    // simple-view<V> && is_reference_v<range_reference_t<V>>;
    {
        xranges::join_view<SimpleParentView> jv;
        static_assert(std::same_as<decltype(jv.begin()),
            decltype(std::as_const(jv).begin())>);
    }

// Check stashing iterators (LWG3698: regex_iterator and join_view don't
// work together very well)
#if RXX_LIBSTDCXX && !RXX_LIBSTDCXX_AFTER(2023, 11, 08)
    if (!std::is_constant_evaluated())
#endif
    {
        xranges::join_view<StashingRange> jv;
        assert(xranges::equal(
            xviews::counted(jv.begin(), 10), std::string_view{"aababcabcd"}));
    }

    // LWG3700: The `const begin` of the `join_view` family does not require
    // `InnerRng` to be a range
    {
        xranges::join_view<ConstNonJoinableRange> jv;
        static_assert(!HasConstBegin<decltype(jv)>);
    }

    // Check example from LWG3700
    {
        auto r = xviews::iota(0, 5) | xviews::split(1);
        auto s = xviews::single(r);
        auto j = s | xviews::join;
        auto f = j.front();
        assert(xranges::equal(f, xviews::single(0)));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
