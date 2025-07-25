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

// template<bool OtherConst>
//   requires sentinel_for<sentinel_t<Base>, iterator_t<maybe-const<OtherConst,
//   V>>>
// friend constexpr bool operator==(const iterator<OtherConst>& x, const
// sentinel& y);

#include "../../test_range.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <compare>
#include <tuple>

using Iterator = random_access_iterator<int*>;
using ConstIterator = random_access_iterator<int const*>;

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <bool Const>
struct ComparableSentinel {

    using Iter = std::conditional_t<Const, ConstIterator, Iterator>;
    Iter iter_;

    explicit ComparableSentinel() = default;
    constexpr explicit ComparableSentinel(Iter const& it) : iter_(it) {}

    constexpr friend bool operator==(
        Iterator const& i, ComparableSentinel const& s) {
        return base(i) == base(s.iter_);
    }

    constexpr friend bool operator==(
        ConstIterator const& i, ComparableSentinel const& s) {
        return base(i) == base(s.iter_);
    }
};

struct ComparableView : IntBufferView {
    using IntBufferView::IntBufferView;

    constexpr auto begin() { return Iterator(buffer_); }
    constexpr auto begin() const { return ConstIterator(buffer_); }
    constexpr auto end() {
        return ComparableSentinel<false>(Iterator(buffer_ + size_));
    }
    constexpr auto end() const {
        return ComparableSentinel<true>(ConstIterator(buffer_ + size_));
    }
};

struct ConstIncompatibleView : xranges::view_base {
    cpp17_input_iterator<int*> begin();
    forward_iterator<int const*> begin() const;
    sentinel_wrapper<cpp17_input_iterator<int*>> end();
    sentinel_wrapper<forward_iterator<int const*>> end() const;
};

constexpr bool test() {
    int buffer1[4] = {1, 2, 3, 4};
    int buffer2[5] = {1, 2, 3, 4, 5};
    int buffer3[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    {
        // simple-view: const and non-const have the same iterator/sentinel type
        xranges::zip_view v{SimpleNonCommon(buffer1), SimpleNonCommon(buffer2),
            SimpleNonCommon(buffer3)};
        static_assert(!xranges::common_range<decltype(v)>);
        static_assert(simple_view<decltype(v)>);

        assert(v.begin() != v.end());
        assert(v.begin() + 1 != v.end());
        assert(v.begin() + 2 != v.end());
        assert(v.begin() + 3 != v.end());
        assert(v.begin() + 4 == v.end());
    }

    {
        // !simple-view: const and non-const have different iterator/sentinel
        // types
        xranges::zip_view v{NonSimpleNonCommon(buffer1),
            SimpleNonCommon(buffer2), SimpleNonCommon(buffer3)};
        static_assert(!xranges::common_range<decltype(v)>);
        static_assert(!simple_view<decltype(v)>);

        assert(v.begin() != v.end());
        assert(v.begin() + 4 == v.end());

        // const_iterator (const int*) converted to iterator (int*)
        assert(v.begin() + 4 == std::as_const(v).end());

        using Iter = xranges::iterator_t<decltype(v)>;
        using ConstIter = xranges::iterator_t<const decltype(v)>;
        static_assert(!std::is_same_v<Iter, ConstIter>);
        using Sentinel = xranges::sentinel_t<decltype(v)>;
        using ConstSentinel = xranges::sentinel_t<const decltype(v)>;
        static_assert(!std::is_same_v<Sentinel, ConstSentinel>);

        static_assert(weakly_equality_comparable_with<Iter, Sentinel>);
        static_assert(!weakly_equality_comparable_with<ConstIter, Sentinel>);
        static_assert(weakly_equality_comparable_with<Iter, ConstSentinel>);
        static_assert(
            weakly_equality_comparable_with<ConstIter, ConstSentinel>);
    }

    {
        // underlying const/non-const sentinel can be compared with both
        // const/non-const iterator
        xranges::zip_view v{ComparableView(buffer1), ComparableView(buffer2)};
        static_assert(!xranges::common_range<decltype(v)>);
        static_assert(!simple_view<decltype(v)>);

        assert(v.begin() != v.end());
        assert(v.begin() + 4 == v.end());
        assert(std::as_const(v).begin() + 4 == v.end());
        assert(std::as_const(v).begin() + 4 == std::as_const(v).end());
        assert(v.begin() + 4 == std::as_const(v).end());

        using Iter = xranges::iterator_t<decltype(v)>;
        using ConstIter = xranges::iterator_t<const decltype(v)>;
        static_assert(!std::is_same_v<Iter, ConstIter>);
        using Sentinel = xranges::sentinel_t<decltype(v)>;
        using ConstSentinel = xranges::sentinel_t<const decltype(v)>;
        static_assert(!std::is_same_v<Sentinel, ConstSentinel>);

        static_assert(weakly_equality_comparable_with<Iter, Sentinel>);
        static_assert(weakly_equality_comparable_with<ConstIter, Sentinel>);
        static_assert(weakly_equality_comparable_with<Iter, ConstSentinel>);
        static_assert(
            weakly_equality_comparable_with<ConstIter, ConstSentinel>);
    }

    {
        // underlying const/non-const sentinel cannot be compared with
        // non-const/const iterator
        xranges::zip_view v{ComparableView(buffer1), ConstIncompatibleView{}};
        static_assert(!xranges::common_range<decltype(v)>);
        static_assert(!simple_view<decltype(v)>);

        using Iter = xranges::iterator_t<decltype(v)>;
        using ConstIter = xranges::iterator_t<const decltype(v)>;
        static_assert(!std::is_same_v<Iter, ConstIter>);
        using Sentinel = xranges::sentinel_t<decltype(v)>;
        using ConstSentinel = xranges::sentinel_t<const decltype(v)>;
        static_assert(!std::is_same_v<Sentinel, ConstSentinel>);

        static_assert(weakly_equality_comparable_with<Iter, Sentinel>);
        static_assert(!weakly_equality_comparable_with<ConstIter, Sentinel>);
        static_assert(!weakly_equality_comparable_with<Iter, ConstSentinel>);
        static_assert(
            weakly_equality_comparable_with<ConstIter, ConstSentinel>);
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
