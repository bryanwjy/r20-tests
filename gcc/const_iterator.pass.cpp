// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2020-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include "rxx/access.h"
#include "rxx/concepts.h"
#include "rxx/iterator.h"
#include "test_iterators.h"

#include <array>
#include <concepts>
#include <ranges>
#include <string_view>
#include <vector>

using rxx::tests::test_bidirectional_range;
using rxx::tests::test_forward_range;
using rxx::tests::test_input_range;
using rxx::tests::test_random_access_range;

namespace ranges = std::ranges;
namespace xranges = rxx::ranges;

template <class Iter, bool Const>
void test01() {
    if constexpr (std::is_pointer_v<Iter>) {
        static_assert(std::same_as<rxx::const_iterator<Iter>,
            std::remove_pointer_t<Iter> const*>);
        static_assert(std::same_as<rxx::const_sentinel<Iter>,
            std::remove_pointer_t<Iter> const*>);
    } else if constexpr (requires { typename Iter::const_iterator_for; }) {
        static_assert(std::same_as<rxx::const_iterator<Iter>,
            typename Iter::const_iterator_for>);
        static_assert(std::same_as<rxx::const_sentinel<Iter>,
            typename Iter::const_iterator_for>);
    } else if constexpr (Const) {
        static_assert(std::same_as<rxx::const_iterator<Iter>, Iter>);
        static_assert(std::same_as<rxx::const_sentinel<Iter>, Iter>);
        static_assert(std::same_as<rxx::iter_const_reference_t<Iter>,
            rxx::iter_reference_t<Iter>>);
    } else {
        using Wrapped = rxx::basic_const_iterator<Iter>;

        static_assert(std::same_as<rxx::const_iterator<Iter>, Wrapped>);
        static_assert(std::same_as<rxx::const_sentinel<Iter>, Wrapped>);
        static_assert(std::same_as<rxx::iter_const_reference_t<Iter>,
            rxx::iter_reference_t<Wrapped>>);

        static_assert(
            std::input_iterator<Iter> == std::input_iterator<Wrapped>);
        static_assert(
            std::forward_iterator<Iter> == std::forward_iterator<Wrapped>);
        static_assert(std::bidirectional_iterator<Iter> ==
            std::bidirectional_iterator<Wrapped>);
        static_assert(std::random_access_iterator<Iter> ==
            std::random_access_iterator<Wrapped>);
    }
}

template <class Range, bool Const>
void test02() {
    if constexpr (std::is_pointer_v<xranges::iterator_t<Range>>) {
        using Iter = xranges::iterator_t<Range>;
        using ConstPtr =
            std::remove_pointer_t<xranges::iterator_t<Range>> const*;

        static_assert(std::same_as<xranges::const_iterator_t<Range>, ConstPtr>);
        if constexpr (xranges::common_range<Range>) {
            static_assert(
                std::same_as<xranges::const_sentinel_t<Range>, ConstPtr>);
        }
        if constexpr (std::is_const_v<std::remove_reference_t<
                          std::iter_reference_t<Iter>>>) {
            static_assert(std::same_as<Iter, ConstPtr>);
        } else {
            using Value = std::remove_reference_t<std::iter_reference_t<Iter>>;
            static_assert(
                std::same_as<std::add_pointer_t<Value const>, ConstPtr>);
        }
    } else if constexpr (requires {
                             typename xranges::iterator_t<
                                 Range>::const_iterator_for;
                         }) {
        using Iter = typename xranges::iterator_t<Range>::const_iterator_for;
        static_assert(std::same_as<xranges::const_iterator_t<Range>, Iter>);
        if constexpr (xranges::common_range<Range>) {
            static_assert(std::same_as<xranges::const_sentinel_t<Range>, Iter>);
        }
        // TODO
    } else if constexpr (Const) {
        static_assert(xranges::constant_range<Range>);
        static_assert(std::same_as<xranges::const_iterator_t<Range>,
            xranges::iterator_t<Range>>);
        static_assert(std::same_as<xranges::const_sentinel_t<Range>,
            xranges::sentinel_t<Range>>);
        static_assert(std::same_as<xranges::range_const_reference_t<Range>,
            xranges::range_reference_t<Range>>);

        static_assert(
            std::same_as<decltype(xranges::cbegin(std::declval<Range&>())),
                decltype(xranges::begin(std::declval<Range&>()))>);
        static_assert(
            std::same_as<decltype(xranges::cend(std::declval<Range&>())),
                decltype(xranges::end(std::declval<Range&>()))>);
    } else {
        static_assert(!xranges::constant_range<Range>);
        using Wrapped = rxx::basic_const_iterator<xranges::iterator_t<Range>>;

        static_assert(std::same_as<xranges::const_iterator_t<Range>, Wrapped>);
        if constexpr (xranges::common_range<Range>)
            static_assert(
                std::same_as<xranges::const_sentinel_t<Range>, Wrapped>);
        static_assert(std::same_as<xranges::range_const_reference_t<Range>,
            rxx::iter_reference_t<Wrapped>>);

        static_assert(
            xranges::input_range<Range> == std::input_iterator<Wrapped>);
        static_assert(
            xranges::forward_range<Range> == std::forward_iterator<Wrapped>);
        static_assert(xranges::bidirectional_range<Range> ==
            std::bidirectional_iterator<Wrapped>);
        static_assert(xranges::random_access_range<Range> ==
            std::random_access_iterator<Wrapped>);

        if constexpr (xranges::constant_range<Range const&>) {
            static_assert(
                std::same_as<decltype(xranges::cbegin(std::declval<Range&>())),
                    decltype(xranges::begin(std::declval<Range const&>()))>);
            static_assert(
                std::same_as<decltype(xranges::cend(std::declval<Range&>())),
                    decltype(xranges::end(std::declval<Range const&>()))>);
        } else {
            static_assert(
                std::same_as<decltype(xranges::cbegin(std::declval<Range&>())),
                    Wrapped>);
            if constexpr (xranges::common_range<Range>)
                static_assert(std::same_as<
                    decltype(xranges::cend(std::declval<Range&>())), Wrapped>);
        }
    }
}

void test03() {
    static_assert(std::same_as<rxx::const_sentinel<std::unreachable_sentinel_t>,
        std::unreachable_sentinel_t>);
}

void test04() {
    // Example from P2836R1
    auto f = [](std::vector<int>::const_iterator i) {};

    auto v = std::vector<int>();
    {
        auto i1 = ranges::cbegin(v); // returns vector<T>::const_iterator
        f(i1);                       // okay
    }

    auto t = v | std::views::take_while([](int const x) { return x < 100; });
    {
        auto i2 = ranges::cbegin(
            t);           // returns basic_const_iterator<vector<T>::iterator>
        f(i2);            // was an error in C++23 before P2836R1
        f(std::move(i2)); // same
    }
}

int main() {
    test01<int*, false>();
    test01<xranges::iterator_t<test_input_range<int>>, false>();
    test01<xranges::iterator_t<test_forward_range<int>>, false>();
    test01<xranges::iterator_t<test_bidirectional_range<int>>, false>();
    test01<xranges::iterator_t<test_random_access_range<int>>, false>();
    test01<std::array<int, 3>::iterator, false>();
    test01<std::vector<bool>::iterator, false>();

    test01<int const*, true>();
    test01<xranges::iterator_t<test_input_range<int const>>, true>();
    test01<xranges::iterator_t<test_forward_range<int const>>, true>();
    test01<xranges::iterator_t<test_bidirectional_range<int const>>, true>();
    test01<xranges::iterator_t<test_random_access_range<int const>>, true>();
    test01<std::array<int const, 3>::iterator, true>();
    test01<std::string_view::iterator, true>();

// Clang was wrong at one point
#ifdef _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL
    test01<std::vector<bool>::const_iterator, true>();
#endif

    test02<int[42], false>();
    test02<test_input_range<int>, false>();
    test02<test_forward_range<int>, false>();
    test02<test_bidirectional_range<int>, false>();
    test02<test_random_access_range<int>, false>();
    test02<std::array<int, 3>, false>();
#ifdef _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL
    test02<std::vector<bool>, false>();
#endif

    test02<int const[42], true>();
    test02<test_input_range<int const>, true>();
    test02<test_forward_range<int const>, true>();
    test02<test_bidirectional_range<int const>, true>();
    test02<test_random_access_range<int const>, true>();
    test02<std::array<int const, 3>, true>();
    test02<std::array<int, 3> const, true>();
    test02<std::string_view, true>();
#ifdef _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL
    test02<std::vector<bool> const, true>();
#endif

    test03();
    test04();
}
