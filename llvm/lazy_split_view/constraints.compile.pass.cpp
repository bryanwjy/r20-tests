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

// This is a compile-only test, so "inline function is not defined" warnings are
// irrelevant. ADDITIONAL_COMPILE_FLAGS(gcc-style-warnings):
// -Wno-undefined-inline

// template<input_range V, forward_range Pattern>
//   requires view<V> && view<Pattern> &&
//            indirectly_comparable<iterator_t<V>, iterator_t<Pattern>,
//            ranges::equal_to> && (forward_range<V> || tiny-range<Pattern>)
// class lazy_split_view;

#include "../test_iterators.h"
#include "rxx/ranges.h"
#include "types.h"

#include <functional>

struct ForwardRange {
    forward_iterator<int*> begin() const;
    forward_iterator<int*> end() const;
};
static_assert(xranges::forward_range<ForwardRange>);

template <class View, class Pattern>
concept CanInstantiate =
    requires { typename xranges::lazy_split_view<View, Pattern>; };

// All constraints satisfied (`View` and `Pattern` are forward views).
namespace test1 {

using View = ForwardView;
using Pattern = ForwardView;
static_assert(xranges::forward_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(CanInstantiate<View, Pattern>);

} // namespace test1

// All constraints satisfied (`View` is an input view and `Pattern` is a tiny
// view).
namespace test2 {

using View = InputView;
using Pattern = ForwardTinyView;
static_assert(xranges::input_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(CanInstantiate<View, Pattern>);

} // namespace test2

// `View` is not an input range.
namespace test3 {

struct AlmostInputIterator {
    using value_type = char;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = int;

    constexpr char const& operator*() const;
    constexpr AlmostInputIterator& operator++();
    constexpr void operator++(int);
    constexpr bool operator==(AlmostInputIterator const&) const;
};

static_assert(std::input_or_output_iterator<AlmostInputIterator>);
static_assert(!std::input_iterator<AlmostInputIterator>);

struct NonInputView : xranges::view_base {
    AlmostInputIterator begin() const;
    AlmostInputIterator end() const;
};

using View = NonInputView;
using Pattern = ForwardTinyView;
static_assert(!xranges::input_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test3

// `View` is not a view.
namespace test4 {

using View = ForwardRange;
using Pattern = ForwardView;
static_assert(xranges::input_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(!xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test4

// `Pattern` is not a forward range.
namespace test5 {

using View = ForwardView;
using Pattern = InputView;
static_assert(xranges::input_range<View>);
static_assert(!xranges::forward_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test5

// Not indirectly comparable.
namespace test6 {

struct Empty {};
struct IntForwardView : xranges::view_base {
    constexpr forward_iterator<Empty*> begin() const { return {}; }
    constexpr forward_iterator<Empty*> end() const { return {}; }
};

using View = ForwardView;
using Pattern = IntForwardView;
static_assert(xranges::input_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(!std::indirectly_comparable<xranges::iterator_t<View>,
              xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test6

// `View` is an input range and `Pattern` is not a tiny range.
namespace test7 {

using View = InputView;
using Pattern = ForwardView;
static_assert(xranges::input_range<View>);
static_assert(!xranges::forward_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(!xranges::details::tiny_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test7

// `View` is an input range and `Pattern` is almost a tiny range, except the
// `size()` function is not `constexpr`.
namespace test8 {

struct AlmostTinyRange : xranges::view_base {
    int* begin() const;
    int* end() const;
    static std::size_t size() { return 1; }
};

using View = InputView;
using Pattern = AlmostTinyRange;
static_assert(xranges::input_range<View>);
static_assert(!xranges::forward_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(!xranges::details::tiny_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test8

// `View` is an input range and `Pattern` is almost a tiny range, except the
// `size()` returns a number `>2`.
namespace test9 {

struct AlmostTinyRange : xranges::view_base {
    int* begin() const;
    int* end() const;
    constexpr static std::size_t size() { return 2; }
};

using View = InputView;
using Pattern = ForwardView;
static_assert(xranges::input_range<View>);
static_assert(!xranges::forward_range<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(!xranges::details::tiny_range<Pattern>);
static_assert(xranges::view<View>);
static_assert(xranges::view<Pattern>);
static_assert(std::indirectly_comparable<xranges::iterator_t<View>,
    xranges::iterator_t<Pattern>, xranges::equal_to>);
static_assert(!CanInstantiate<View, Pattern>);

} // namespace test9
