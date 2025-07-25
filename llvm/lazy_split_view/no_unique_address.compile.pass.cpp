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

// class lazy_split_view {
//   _LIBCPP_NO_UNIQUE_ADDRESS _View __base_ = _View();
//   _LIBCPP_NO_UNIQUE_ADDRESS _Pattern __pattern_ = _Pattern();
// };

#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <string_view>

// Verify the optimization that, if `View` is a forward range, the
// `lazy_split_view` itself doesn't store the `current` iterator (instead, it's
// stored in the `outer-iterator`).
//
// Note that the Standard marks all data members of `lazy_split_view` as
// "exposition only", so this test has to be libc++-specific.
namespace test1 {

using SplitView = xranges::lazy_split_view<std::string_view, std::string_view>;
// The `lazy_split_view` only stores the `View` and the `Pattern`, not an
// iterator.
static_assert(sizeof(SplitView) == sizeof(std::string_view) * 2);

} // namespace test1

// Verify the optimization that, if `View` is an input range, the
// `outer-iterator` doesn't store the `current` iterator (instead, it's stored
// in the `lazy_split_view` itself).
//
// Note that the Standard marks all data members of `outer-iterator` as
// "exposition only", so this test has to be libc++-specific.
namespace test2 {

struct InputView : xranges::view_base {
    int x;
    cpp20_input_iterator<int*> begin() const;
    sentinel_wrapper<cpp20_input_iterator<int*>> end() const;
};
static_assert(xranges::input_range<InputView>);
static_assert(!xranges::forward_range<InputView>);
static_assert(xranges::view<InputView>);

struct TinyView : xranges::view_base {
    int x;
    int* begin() const;
    int* end() const;
    constexpr static std::size_t size() { return 1; }
};
static_assert(xranges::forward_range<TinyView>);
static_assert(xranges::details::tiny_range<TinyView>);
static_assert(xranges::view<TinyView>);

using SplitView = xranges::lazy_split_view<InputView, TinyView>;
using OuterIter = xranges::iterator_t<SplitView>;
// The `outer-iterator` only stores a pointer to the parent and a boolean
// (aligned to the size of a pointer), not an iterator.
static_assert(sizeof(OuterIter) == sizeof(void*) * 2);

} // namespace test2

// Verify the libc++-specific optimization that empty `View` and `Pattern` use
// the `[[no_unique_address]]` attribute. Both `View` and `Pattern` are forward
// views.
namespace test3 {

struct EmptyView1 : xranges::view_base {
    int* begin() const;
    int* end() const;
};
static_assert(xranges::forward_range<EmptyView1>);
static_assert(xranges::view<EmptyView1>);

// Note: it's important to inherit `EmptyView1` and `EmptyView2` from different
// bases, otherwise they still cannot share the same address regardless of
// whether `[[no_unique_address]]` is used.
struct EmptyView2 : xranges::view_interface<EmptyView2> {
    int* begin() const;
    int* end() const;
};
static_assert(xranges::forward_range<EmptyView2>);
static_assert(xranges::view<EmptyView2>);

static_assert(sizeof(xranges::lazy_split_view<EmptyView1, EmptyView2>) == 1);

} // namespace test3

// Verify the libc++-specific optimization that empty `View` and `Pattern` use
// the `[[no_unique_address]]` attribute. `View` is an input view and `Pattern`
// is a tiny view.
namespace test4 {

struct EmptyInputView : xranges::view_base {
    cpp20_input_iterator<int*> begin() const;
    sentinel_wrapper<cpp20_input_iterator<int*>> end() const;
};
static_assert(xranges::input_range<EmptyInputView>);
static_assert(!xranges::forward_range<EmptyInputView>);
static_assert(xranges::view<EmptyInputView>);

struct EmptyTinyView : xranges::view_base {
    int* begin() const;
    int* end() const;
    constexpr static std::size_t size() { return 1; }
};
static_assert(xranges::forward_range<EmptyTinyView>);
static_assert(xranges::details::tiny_range<EmptyTinyView>);
static_assert(xranges::view<EmptyTinyView>);

static_assert(sizeof(xranges::lazy_split_view<EmptyInputView, EmptyTinyView>) ==
    sizeof(xranges::details::non_propagating_cache<
        xranges::iterator_t<EmptyInputView>>));

} // namespace test4
