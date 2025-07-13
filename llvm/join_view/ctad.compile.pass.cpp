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

// template<class R>
//   explicit join_view(R&&) -> join_view<views::all_t<R>>;

#include "rxx/join_view.h"

#include <ranges>
#include <utility>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;
struct Child {
    int* begin() const;
    int* end() const;
};

struct View : std::ranges::view_base {
    Child* begin() const;
    Child* end() const;
};

struct Range {
    Child* begin() const;
    Child* end() const;
};

struct BorrowedRange {
    Child* begin() const;
    Child* end() const;
};
template <>
inline constexpr bool std::ranges::enable_borrowed_range<BorrowedRange> = true;

struct NestedChildren : std::ranges::view_base {
    View* begin() const;
    View* end() const;
};

void testCTAD() {
    View v;
    Range r;
    BorrowedRange br;

    static_assert(std::same_as<decltype(xranges::join_view(v)),
        xranges::join_view<View>>);
    static_assert(std::same_as<decltype(xranges::join_view(std::move(v))),
        xranges::join_view<View>>);
    static_assert(std::same_as<decltype(xranges::join_view(r)),
        xranges::join_view<std::ranges::ref_view<Range>>>);
    static_assert(std::same_as<decltype(xranges::join_view(std::move(r))),
        xranges::join_view<std::ranges::owning_view<Range>>>);
    static_assert(std::same_as<decltype(xranges::join_view(br)),
        xranges::join_view<std::ranges::ref_view<BorrowedRange>>>);
    static_assert(std::same_as<decltype(xranges::join_view(std::move(br))),
        xranges::join_view<std::ranges::owning_view<BorrowedRange>>>);

    NestedChildren n;
    xranges::join_view jv(n);

    // CTAD generated from the copy constructor instead of joining the join_view
    static_assert(std::same_as<decltype(xranges::join_view(jv)), decltype(jv)>);

    // CTAD generated from the move constructor instead of joining the join_view
    static_assert(std::same_as<decltype(xranges::join_view(std::move(jv))),
        decltype(jv)>);
}
