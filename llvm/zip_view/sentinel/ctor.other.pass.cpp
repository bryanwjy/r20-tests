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

// constexpr sentinel(sentinel<!Const> s);

#include "../types.h"
#include "rxx/ranges/zip_view.h"

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

struct NonSimpleNonCommonConvertibleView : IntBufferView {
    using IntBufferView::IntBufferView;

    constexpr int* begin() { return buffer_; }
    constexpr int const* begin() const { return buffer_; }
    constexpr convertible_sentinel_wrapper<int*> end() {
        return convertible_sentinel_wrapper<int*>(buffer_ + size_);
    }
    constexpr convertible_sentinel_wrapper<int const*> end() const {
        return convertible_sentinel_wrapper<int const*>(buffer_ + size_);
    }
};

static_assert(!xranges::common_range<NonSimpleNonCommonConvertibleView>);
static_assert(xranges::random_access_range<NonSimpleNonCommonConvertibleView>);
static_assert(!xranges::sized_range<NonSimpleNonCommonConvertibleView>);
static_assert(
    std::convertible_to<xranges::sentinel_t<NonSimpleNonCommonConvertibleView>,
        xranges::sentinel_t<NonSimpleNonCommonConvertibleView const>>);
static_assert(!simple_view<NonSimpleNonCommonConvertibleView>);

constexpr bool test() {
    int buffer1[4] = {1, 2, 3, 4};
    int buffer2[5] = {1, 2, 3, 4, 5};
    xranges::zip_view v{NonSimpleNonCommonConvertibleView(buffer1),
        NonSimpleNonCommonConvertibleView(buffer2)};
    static_assert(!xranges::common_range<decltype(v)>);
    auto sent1 = v.end();
    xranges::sentinel_t<const decltype(v)> sent2 = sent1;
    static_assert(!std::is_same_v<decltype(sent1), decltype(sent2)>);

    assert(v.begin() != sent2);
    assert(std::as_const(v).begin() != sent2);
    assert(v.begin() + 4 == sent2);
    assert(std::as_const(v).begin() + 4 == sent2);

    // Cannot create a non-const iterator from a const iterator.
    static_assert(!std::constructible_from<decltype(sent1), decltype(sent2)>);
    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
