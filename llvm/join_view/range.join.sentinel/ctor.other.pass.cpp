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

// constexpr sentinel(sentinel<!Const> s);
//             requires Const && convertible_to<sentinel_t<V>,
//             sentinel_t<Base>>;

#include "../../test_range.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <cassert>
#include <ranges>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
template <class T>
struct convertible_sentinel_wrapper {
    explicit convertible_sentinel_wrapper() = default;
    constexpr convertible_sentinel_wrapper(T const& it) : it_(it) {}

    template <class U>
    requires std::convertible_to<U const&, T>
    constexpr convertible_sentinel_wrapper(
        convertible_sentinel_wrapper<U> const& other)
        : it_(other.it_) {}

    constexpr friend bool operator==(
        convertible_sentinel_wrapper const& self, T const& other) {
        return self.it_ == other;
    }
    T it_;
};

struct ConstConvertibleView : BufferView<BufferView<int*>*> {
    using BufferView<BufferView<int*>*>::BufferView;

    using sentinel = convertible_sentinel_wrapper<BufferView<int*>*>;
    using const_sentinel =
        convertible_sentinel_wrapper<BufferView<int*> const*>;

    constexpr BufferView<int*>* begin() { return data_; }
    constexpr BufferView<int*> const* begin() const { return data_; }
    constexpr sentinel end() { return sentinel(data_ + size_); }
    constexpr const_sentinel end() const {
        return const_sentinel(data_ + size_);
    }
};
static_assert(!xranges::common_range<ConstConvertibleView>);
static_assert(std::convertible_to<xranges::sentinel_t<ConstConvertibleView>,
    xranges::sentinel_t<ConstConvertibleView const>>);
static_assert(!simple_view<ConstConvertibleView>);

constexpr bool test() {
    int buffer[4][4] = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    {
        BufferView<int*> inners[] = {buffer[0], buffer[1], buffer[2]};
        ConstConvertibleView outer(inners);
        xranges::join_view jv(outer);
        auto sent1 = jv.end();
        xranges::sentinel_t<const decltype(jv)> sent2 = sent1;
        assert(std::as_const(jv).begin() != sent2);
        assert(xranges::next(std::as_const(jv).begin(), 12) == sent2);

        // We cannot create a non-const sentinel from a const sentinel.
        static_assert(
            !std::constructible_from<decltype(sent1), decltype(sent2)>);
    }

    {
        // cannot create a const sentinel from a non-const sentinel if the
        // underlying const sentinel cannot be created from the underlying
        // non-const sentinel
        using Inner = BufferView<int*>;
        using ConstInconvertibleOuter =
            BufferView<forward_iterator<Inner const*>,
                sentinel_wrapper<forward_iterator<Inner const*>>,
                bidirectional_iterator<Inner*>,
                sentinel_wrapper<bidirectional_iterator<Inner*>>>;
        using JoinView = xranges::join_view<ConstInconvertibleOuter>;
        using sentinel_t = xranges::sentinel_t<JoinView>;
        using const_sentinel_t = xranges::sentinel_t<JoinView const>;
        static_assert(!std::constructible_from<sentinel_t, const_sentinel_t>);
        static_assert(!std::constructible_from<const_sentinel_t, sentinel_t>);
    }
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
