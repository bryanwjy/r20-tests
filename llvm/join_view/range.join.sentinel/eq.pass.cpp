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

// template<bool OtherConst>
//   requires sentinel_for<sentinel_t<Base>, iterator_t<maybe-const<OtherConst,
//   V>>>
// friend constexpr bool operator==(const iterator<OtherConst>& x, const
// sentinel& y);

#include "../../test_range.h"
#include "../types.h"
#include "rxx/join_view.h"

#include <cassert>
#include <concepts>
#include <functional>
#include <ranges>
#include <type_traits>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
using Iterator = random_access_iterator<BufferView<int*>*>;
using ConstIterator = random_access_iterator<BufferView<int*> const*>;

template <bool Const>
struct ConstComparableSentinel {

    using Iter = std::conditional_t<Const, ConstIterator, Iterator>;
    Iter iter_;

    explicit ConstComparableSentinel() = default;
    constexpr explicit ConstComparableSentinel(Iter const& it) : iter_(it) {}

    constexpr friend bool operator==(
        Iterator const& i, ConstComparableSentinel const& s) {
        return base(i) == base(s.iter_);
    }

    constexpr friend bool operator==(
        ConstIterator const& i, ConstComparableSentinel const& s) {
        return base(i) == base(s.iter_);
    }
};

struct ConstComparableView : BufferView<BufferView<int*>*> {
    using BufferView<BufferView<int*>*>::BufferView;

    constexpr auto begin() { return Iterator(data_); }
    constexpr auto begin() const { return ConstIterator(data_); }
    constexpr auto end() {
        return ConstComparableSentinel<false>(Iterator(data_ + size_));
    }
    constexpr auto end() const {
        return ConstComparableSentinel<true>(ConstIterator(data_ + size_));
    }
};

static_assert(
    weakly_equality_comparable_with<xranges::iterator_t<ConstComparableView>,
        xranges::sentinel_t<ConstComparableView const>>);
static_assert(weakly_equality_comparable_with<
    xranges::iterator_t<ConstComparableView const>,
    xranges::sentinel_t<ConstComparableView>>);

constexpr bool test() {
    int buffer[4][4] = {
        {1111, 2222, 3333, 4444},
        { 555,  666,  777,  888},
        {  99, 1010, 1111, 1212},
        {  13,   14,   15,   16}
    };

    // test iterator<false> == sentinel<false>
    {
        ChildView children[4] = {ChildView(buffer[0]), ChildView(buffer[1]),
            ChildView(buffer[2]), ChildView(buffer[3])};
        auto jv = xranges::join_view(ParentView(children));
        assert(jv.end() == std::ranges::next(jv.begin(), 16));
    }

    // test iterator<false> == sentinel<true>
    {
        ChildView children[4] = {ChildView(buffer[0]), ChildView(buffer[1]),
            ChildView(buffer[2]), ChildView(buffer[3])};
        using ParentT = std::remove_all_extents_t<decltype(children)>;
        auto jv = xranges::join_view(ForwardParentView<ParentT>(children));
        assert(std::as_const(jv).end() == std::ranges::next(jv.begin(), 16));
    }

    // test iterator<true> == sentinel<true>
    {
        CopyableChild children[4] = {CopyableChild(buffer[0]),
            CopyableChild(buffer[1]), CopyableChild(buffer[2]),
            CopyableChild(buffer[3])};
        using ParentT = std::remove_all_extents_t<decltype(children)>;
        auto const jv =
            xranges::join_view(ForwardParentView<ParentT>(children));
        assert(jv.end() == std::ranges::next(jv.begin(), 16));
    }

    // test iterator<Const> == sentinel<!Const>
    {
        BufferView<int*> inners[] = {buffer[0], buffer[1]};
        ConstComparableView outer(inners);
        auto jv = xranges::join_view(outer);
        assert(jv.end() == std::ranges::next(jv.begin(), 8));
        assert(std::as_const(jv).end() == std::ranges::next(jv.begin(), 8));
        assert(jv.end() == std::ranges::next(std::as_const(jv).begin(), 8));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
