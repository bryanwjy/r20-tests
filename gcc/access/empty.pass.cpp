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

// This used to be:
// PR testsuite/101782
// attribute-specifier-seq cannot follow requires-clause with -fconcepts-ts
#include "../test_iterators.h"
#include "rxx/ranges.h"

#include <cassert>

namespace xranges = rxx::ranges;
namespace xviews = rxx::views;

static_assert(rxx::tests::is_customization_point_object(std::ranges::empty));

using std::same_as;

void test01() {
    struct R {
        constexpr int empty() const& { return 0; }
        constexpr void const* empty() const&& { return this; }
    };
    constexpr R r;
    static_assert(!std::ranges::empty(r));
    static_assert(same_as<decltype(std::ranges::empty(r)), bool>);
    // PR libstdc++/100824
    // ranges::empty should treat the subexpression as an lvalue
    static_assert(!std::ranges::empty(std::move(r)));
    static_assert(same_as<decltype(std::ranges::empty(std::move(r))), bool>);
}

void test02() {
    using rxx::tests::forward_iterator_wrapper;
    using rxx::tests::input_iterator_wrapper;
    using rxx::tests::output_iterator_wrapper;
    using rxx::tests::random_access_iterator_wrapper;
    using rxx::tests::test_range;
    using rxx::tests::test_sized_range;

    int a[] = {0, 1};
    assert(!std::ranges::empty(a));

    test_range<int, random_access_iterator_wrapper> r(a);
    assert(!std::ranges::empty(r));

    test_range<int, forward_iterator_wrapper> i(a);
    assert(!std::ranges::empty(i));

    test_sized_range<int, random_access_iterator_wrapper> sr(a);
    assert(!std::ranges::empty(sr));

    test_sized_range<int, input_iterator_wrapper> si(a);
    assert(!std::ranges::empty(si));

    test_sized_range<int, output_iterator_wrapper> so(a);
    assert(!std::ranges::empty(so));
}

void test03() {
    // PR libstdc++/100824
    // ranges::empty should treat the subexpression as an lvalue

    struct R {
        constexpr bool empty() & { return true; }
    };
    static_assert(std::ranges::empty(R{}));

    struct R2 {
        constexpr unsigned size() & { return 0; }
    };
    static_assert(std::ranges::empty(R2{}));
}

void test04() {
    struct E1 {
        bool empty() const noexcept { return {}; }
    };

    static_assert(noexcept(std::ranges::empty(E1{})));

    struct E2 {
        bool empty() const noexcept(false) { return {}; }
    };

    static_assert(!noexcept(std::ranges::empty(E2{})));

    struct E3 {
        struct B {
            explicit operator bool() const noexcept(false) { return true; }
        };

        B empty() const noexcept { return {}; }
    };

    static_assert(!noexcept(std::ranges::empty(E3{})));
}

template <typename T>
concept has_empty = requires(T& t) { std::ranges::empty(t); };

// If T is an array of unknown bound, ranges::empty(E) is ill-formed.
static_assert(!has_empty<int[]>);
static_assert(!has_empty<int (&)[]>);
static_assert(!has_empty<int[][2]>);
struct Incomplete;
static_assert(!has_empty<Incomplete[]>);

int main() {
    test01();
    test02();
    test03();
    test04();
}
