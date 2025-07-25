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

// zip_view() = default;

#include "rxx/ranges.h"

#include <cassert>
#include <type_traits>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

constexpr int buff[] = {1, 2, 3};

struct DefaultConstructibleView : xranges::view_base {
    constexpr DefaultConstructibleView() : begin_(buff), end_(buff + 3) {}
    constexpr int const* begin() const { return begin_; }
    constexpr int const* end() const { return end_; }

private:
    int const* begin_;
    int const* end_;
};

struct NoDefaultCtrView : xranges::view_base {
    NoDefaultCtrView() = delete;
    int* begin() const;
    int* end() const;
};

// The default constructor requires all underlying views to be default
// constructible. It is implicitly required by the tuple's constructor. If any
// of the iterators are not default constructible, zip iterator's =default would
// be implicitly deleted.
static_assert(std::is_default_constructible_v<
    xranges::zip_view<DefaultConstructibleView>>);
static_assert(std::is_default_constructible_v<
    xranges::zip_view<DefaultConstructibleView, DefaultConstructibleView>>);
static_assert(!std::is_default_constructible_v<
              xranges::zip_view<DefaultConstructibleView, NoDefaultCtrView>>);
static_assert(!std::is_default_constructible_v<
              xranges::zip_view<NoDefaultCtrView, NoDefaultCtrView>>);
static_assert(
    !std::is_default_constructible_v<xranges::zip_view<NoDefaultCtrView>>);

constexpr bool test() {
    {
        using View = xranges::zip_view<DefaultConstructibleView,
            DefaultConstructibleView>;
        View v = View(); // the default constructor is not explicit
        assert(v.size() == 3);
        auto it = v.begin();
        using Value = std::tuple<int const&, int const&>;
        assert(*it++ == Value(buff[0], buff[0]));
        assert(*it++ == Value(buff[1], buff[1]));
        assert(*it == Value(buff[2], buff[2]));
    }

    return true;
}

int main(int, char**) {
    test();
    static_assert(test());

    return 0;
}
