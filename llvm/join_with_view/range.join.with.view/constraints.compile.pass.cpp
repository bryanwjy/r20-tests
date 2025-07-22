// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// <ranges>

// template <input_range V, forward_range Pattern>
//  requires view<V> && input_range<range_reference_t<V>> && view<Pattern> &&
//           compatible-joinable-ranges<range_reference_t<V>, Pattern>
// class join_with_view;

#include "../../test_iterators.h"
#include "../types.h"
#include "rxx/ranges.h"

#include <cstddef>
#include <vector>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

template <class View, class Pattern>
concept CanFormJoinWithView =
    requires { typename xranges::join_with_view<View, Pattern>; };

// join_with_view is not valid when `V` is not an input_range
namespace test_when_view_is_not_an_input_range {
struct View : xranges::view_base {
    using It = cpp20_output_iterator<std::vector<int>*>;
    It begin();
    sentinel_wrapper<It> end();
};

struct Pattern : xranges::view_base {
    int* begin();
    int* end();
};

static_assert(xranges::range<View>);
static_assert(!xranges::input_range<View>);
static_assert(xranges::view<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(!CanFormJoinWithView<View, Pattern>);
} // namespace test_when_view_is_not_an_input_range

// join_with_view is not valid when `Pattern` is not a forward_range
namespace test_when_pattern_is_not_a_forward_range {
struct View : xranges::view_base {
    std::vector<float>* begin();
    std::vector<float>* end();
};

struct Pattern : xranges::view_base {
    using It = cpp20_input_iterator<float*>;
    It begin();
    sentinel_wrapper<It> end();
};

static_assert(xranges::input_range<View>);
static_assert(xranges::view<View>);
static_assert(!xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(!CanFormJoinWithView<View, Pattern>);
} // namespace test_when_pattern_is_not_a_forward_range

// join_with_view is not valid when `V` does not model xranges::view
namespace test_when_view_does_not_model_view {
struct View {
    std::vector<double>* begin();
    std::vector<double>* end();
};

struct Pattern : xranges::view_base {
    double* begin();
    double* end();
};

static_assert(xranges::input_range<View>);
static_assert(!xranges::view<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(!CanFormJoinWithView<View, Pattern>);
} // namespace test_when_view_does_not_model_view

// join_with_view is not valid when `range_reference_t` of `V` is not an
// input_range
namespace test_when_range_reference_t_of_view_is_not_an_input_range {
struct InnerRange {
    using It = cpp20_output_iterator<long*>;
    It begin();
    sentinel_wrapper<It> end();
};

struct View : xranges::view_base {
    InnerRange* begin();
    InnerRange* end();
};

struct Pattern : xranges::view_base {
    long* begin();
    long* end();
};

static_assert(xranges::range<InnerRange>);
static_assert(!xranges::input_range<InnerRange>);
static_assert(xranges::input_range<View>);
static_assert(xranges::view<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(!CanFormJoinWithView<View, Pattern>);
} // namespace test_when_range_reference_t_of_view_is_not_an_input_range

// join_with_view is not valid when `Pattern` does not model xranges::view
namespace test_when_pattern_does_not_model_view {
struct View : xranges::view_base {
    std::vector<short>* begin();
    std::vector<short>* end();
};

struct Pattern {
    short* begin();
    short* end();
};

static_assert(xranges::input_range<View>);
static_assert(xranges::view<View>);
static_assert(xranges::forward_range<Pattern>);
static_assert(!xranges::view<Pattern>);
static_assert(!CanFormJoinWithView<View, Pattern>);
} // namespace test_when_pattern_does_not_model_view

// join_with_view is not valid when `range_reference_t<View>` and pattern
// does not model together compatible-joinable-ranges
namespace test_when_used_ranges_are_not_concatable {
using xranges::range_reference_t;
using xranges::range_rvalue_reference_t;
using xranges::range_value_t;

template <class InnerRange>
struct View : xranges::view_base {
    InnerRange* begin();
    InnerRange* end();
};

namespace no_concat_reference_t {
struct ValueType {};

struct InnerRange {
    struct It {
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        struct reference {
            operator value_type();
        };

        It& operator++();
        void operator++(int);
        reference operator*() const;
    };

    It begin();
    sentinel_wrapper<It> end();
};

struct Pattern : xranges::view_base {
    struct It {
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        struct reference {
            operator value_type();
        };

        It& operator++();
        It operator++(int);
        reference operator*() const;
        bool operator==(It const&) const;
        friend value_type&& iter_move(It const&);
    };

    It begin();
    It end();
};

static_assert(xranges::input_range<InnerRange>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(!std::common_reference_with<range_reference_t<InnerRange>,
              range_reference_t<Pattern>>);
static_assert(
    std::common_with<range_value_t<InnerRange>, range_value_t<Pattern>>);
static_assert(std::common_reference_with<range_rvalue_reference_t<InnerRange>,
    range_rvalue_reference_t<Pattern>>);
static_assert(!CanFormJoinWithView<View<InnerRange>, Pattern>);
} // namespace no_concat_reference_t

namespace no_concat_value_t {
struct InnerRange {
    struct It {
        using difference_type = std::ptrdiff_t;
        struct value_type {};

        struct reference {
            operator value_type();
            operator float();
        };

        It& operator++();
        void operator++(int);
        reference operator*() const;
    };

    It begin();
    sentinel_wrapper<It> end();
};

struct Pattern : xranges::view_base {
    float const* begin();
    float const* end();
};

static_assert(xranges::input_range<InnerRange>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(std::common_reference_with<range_reference_t<InnerRange>,
    range_reference_t<Pattern>>);
static_assert(
    !std::common_with<range_value_t<InnerRange>, range_value_t<Pattern>>);
static_assert(std::common_reference_with<range_rvalue_reference_t<InnerRange>,
    range_rvalue_reference_t<Pattern>>);
static_assert(!CanFormJoinWithView<View<InnerRange>, Pattern>);
} // namespace no_concat_value_t

namespace no_concat_rvalue_reference_t {
struct InnerRange {
    using It = cpp20_input_iterator<int*>;
    It begin();
    sentinel_wrapper<It> end();
};

struct Pattern : xranges::view_base {
    struct It {
        using difference_type = std::ptrdiff_t;
        struct value_type {
            operator int() const;
        };

        struct rvalue_reference {
            operator value_type();
        };

        It& operator++();
        It operator++(int);
        value_type& operator*() const;
        bool operator==(It const&) const;
        friend rvalue_reference iter_move(It const&);
    };

    It begin();
    It end();
};

static_assert(xranges::input_range<InnerRange>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(std::common_reference_with<range_reference_t<InnerRange>,
    range_reference_t<Pattern>>);
static_assert(
    std::common_with<range_value_t<InnerRange>, range_value_t<Pattern>>);
static_assert(!std::common_reference_with<range_rvalue_reference_t<InnerRange>,
              range_rvalue_reference_t<Pattern>>);
static_assert(!CanFormJoinWithView<View<InnerRange>, Pattern>);
} // namespace no_concat_rvalue_reference_t

namespace not_concat_indirectly_readable { // Required after LWG-4074
                                           // ("compatible-joinable-ranges is
                                           // underconstrained")
struct InnerRange {
    using It = cpp20_input_iterator<int*>;
    It begin();
    sentinel_wrapper<It> end();
};

struct Pattern : xranges::view_base {
    lwg4074::Iter begin();
    lwg4074::Iter end();
};

static_assert(xranges::input_range<InnerRange>);
static_assert(xranges::forward_range<Pattern>);
static_assert(xranges::view<Pattern>);
static_assert(std::common_reference_with<range_reference_t<InnerRange>,
    range_reference_t<Pattern>>);
static_assert(
    std::common_with<range_value_t<InnerRange>, range_value_t<Pattern>>);
static_assert(std::common_reference_with<range_rvalue_reference_t<InnerRange>,
    range_rvalue_reference_t<Pattern>>);
static_assert(
    !xranges::details::concat_indirectly_readable<InnerRange, Pattern>);
static_assert(!CanFormJoinWithView<View<InnerRange>, Pattern>);
} // namespace not_concat_indirectly_readable
} // namespace test_when_used_ranges_are_not_concatable
