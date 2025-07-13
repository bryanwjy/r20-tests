//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LIBCXX_TEST_SUPPORT_TEST_RANGE_H
#define LIBCXX_TEST_SUPPORT_TEST_RANGE_H

#include "test_iterators.h"

#include <concepts>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>

struct sentinel {
    bool operator==(std::input_or_output_iterator auto const&) const;
};

template <template <class...> class I, class T = int>
requires std::input_or_output_iterator<I<T*>>
struct test_range {
    I<T*> begin();
    I<T const*> begin() const;
    sentinel end();
    sentinel end() const;
};

template <template <class...> class I, class T = int>
requires std::input_or_output_iterator<I<T*>>
struct test_non_const_range {
    I<T*> begin();
    sentinel end();
};

template <template <class...> class I, class T = int>
requires std::input_or_output_iterator<I<T*>>
struct test_common_range {
    I<T*> begin();
    I<T const*> begin() const;
    I<T*> end();
    I<T const*> end() const;
};

template <template <class...> class I, class T = int>
requires std::input_or_output_iterator<I<T*>>
struct test_non_const_common_range {
    I<T*> begin();
    I<T*> end();
};

template <template <class...> class I, class T = int>
requires std::input_or_output_iterator<I<T*>>
struct test_view : std::ranges::view_base {
    I<T*> begin();
    I<T const*> begin() const;
    sentinel end();
    sentinel end() const;
};

template <class T = int>
struct BorrowedRange {
    T* begin() const;
    T* end() const;
    BorrowedRange(BorrowedRange&&) = delete;
};
template <class T>
inline constexpr bool std::ranges::enable_borrowed_range<BorrowedRange<T>> =
    true;
static_assert(!xranges::view<BorrowedRange<>>);
static_assert(xranges::borrowed_range<BorrowedRange<>>);

using BorrowedView = std::ranges::empty_view<int>;
static_assert(xranges::view<BorrowedView>);
static_assert(xranges::borrowed_range<BorrowedView>);

using NonBorrowedView = std::ranges::single_view<int>;
static_assert(xranges::view<NonBorrowedView>);
static_assert(!xranges::borrowed_range<NonBorrowedView>);

template <class Range>
concept simple_view = xranges::view<Range> && std::ranges::range<Range const> &&
    std::same_as<xranges::iterator_t<Range>,
        xranges::iterator_t<Range const>> &&
    std::same_as<xranges::sentinel_t<Range>, xranges::sentinel_t<Range const>>;

template <class View, class T>
concept CanBePiped = requires(View&& view, T&& t) {
    { std::forward<View>(view) | std::forward<T>(t) };
};

// See [concept.equalitycomparable]
template <class T, class U>
concept weakly_equality_comparable_with = requires(
    std::remove_reference_t<T> const& t, std::remove_reference_t<U> const& u) {
    { t == u } -> std::same_as<bool>;
    { t != u } -> std::same_as<bool>;
    { u == t } -> std::same_as<bool>;
    { u != t } -> std::same_as<bool>;
};

#endif // LIBCXX_TEST_SUPPORT_TEST_RANGE_H
