// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <ranges>

// Check constraints on the type itself.
//
// template <forward_range View, indirect_binary_predicate<iterator_t<View>,
// iterator_t<View>> Pred>
//   requires view<View> && is_object_v<Pred>
// class chunk_by_view;

#include "../../almost_satisfies_types.h"
#include "rxx/ranges/chunk_by_view.h"
#include "rxx/ranges/view_base.h"

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace xranges = __RXX ranges;

template <class View, class Pred>
concept CanFormChunkByView =
    requires { typename xranges::chunk_by_view<View, Pred>; };

// chunk_by_view is not valid when the view is not a forward_range
namespace test_when_view_is_not_a_forward_range {

struct View : xranges::view_base {
    ForwardIteratorNotDerivedFrom begin() const;
    ForwardIteratorNotDerivedFrom end() const;
};
struct Pred {
    bool operator()(int, int) const;
};

static_assert(!xranges::forward_range<View>);
static_assert(std::indirect_binary_predicate<Pred, int*, int*>);
static_assert(xranges::view<View>);
static_assert(std::is_object_v<Pred>);
static_assert(!CanFormChunkByView<View, Pred>);

} // namespace test_when_view_is_not_a_forward_range

// chunk_by_view is not valid when the predicate is not
// indirect_binary_predicate
namespace test_when_the_predicate_is_not_indirect_binary_predicate {

struct View : xranges::view_base {
    int* begin() const;
    int* end() const;
};
struct Pred {};

static_assert(xranges::forward_range<View>);
static_assert(!std::indirect_binary_predicate<Pred, int*, int*>);
static_assert(xranges::view<View>);
static_assert(std::is_object_v<Pred>);
static_assert(!CanFormChunkByView<View, Pred>);

} // namespace test_when_the_predicate_is_not_indirect_binary_predicate

// chunk_by_view is not valid when the view is not a view
namespace test_when_the_view_param_is_not_a_view {

struct View {
    int* begin() const;
    int* end() const;
};
struct Pred {
    bool operator()(int, int) const;
};

static_assert(xranges::input_range<View>);
static_assert(std::indirect_binary_predicate<Pred, int*, int*>);
static_assert(!xranges::view<View>);
static_assert(std::is_object_v<Pred>);
static_assert(!CanFormChunkByView<View, Pred>);

} // namespace test_when_the_view_param_is_not_a_view

// chunk_by_view is not valid when the predicate is not an object type
namespace test_when_the_predicate_is_not_an_object_type {

struct View : xranges::view_base {
    int* begin() const;
    int* end() const;
};
using Pred = bool (&)(int, int);

static_assert(xranges::input_range<View>);
static_assert(std::indirect_binary_predicate<Pred, int*, int*>);
static_assert(xranges::view<View>);
static_assert(!std::is_object_v<Pred>);
static_assert(!CanFormChunkByView<View, Pred>);

} // namespace test_when_the_predicate_is_not_an_object_type

// chunk_by_view is valid when all the constraints are satisfied (test the test)
namespace test_when_all_the_constraints_are_satisfied {

struct View : xranges::view_base {
    int* begin() const;
    int* end() const;
};
struct Pred {
    bool operator()(int, int) const;
};

static_assert(xranges::input_range<View>);
static_assert(std::indirect_binary_predicate<Pred, int*, int*>);
static_assert(xranges::view<View>);
static_assert(std::is_object_v<Pred>);
static_assert(CanFormChunkByView<View, Pred>);

} // namespace test_when_all_the_constraints_are_satisfied
