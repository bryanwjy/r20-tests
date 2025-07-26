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

static_assert(rxx::tests::is_customization_point_object(xranges::data));

template <typename T>
concept has_data = requires(T&& t) { xranges::data(std::forward<T>(t)); };

void test01() {
    struct R {
        int i = 0;
        int j = 0;
        int* data() { return &j; }
        R const* data() const noexcept { return nullptr; }
    };
    static_assert(has_data<R&>);
    static_assert(has_data<R const&>);
    R r;
    R const& c = r;
    assert(xranges::data(r) == &r.j);
    static_assert(!noexcept(xranges::data(r)));
    assert(xranges::data(c) == (R*)nullptr);
    static_assert(noexcept(xranges::data(c)));

    // not lvalues and not borrowed ranges
    static_assert(!has_data<R>);
    static_assert(!has_data<R const>);
}

void test02() {
    int a[] = {0, 1};
    assert(xranges::data(a) == a + 0);

    rxx::tests::test_range<int, rxx::tests::contiguous_iterator_wrapper> r(a);
    assert(xranges::data(r) == std::to_address(xranges::begin(r)));

    static_assert(has_data<int(&)[2]>);
    static_assert(has_data<decltype(r)&>);
    static_assert(!has_data<int(&&)[2]>);
    static_assert(!has_data<decltype(r)&&>);
}

struct R3 {
    static inline int i;
    static inline long l;

    int* data() & { return &i; }
    friend long* begin(const R3& r) { return &l; }
    friend short const* begin(const R3&&); // not defined
};

template <>
constexpr bool std::ranges::enable_borrowed_range<R3> = true;

void test03() {
    static_assert(has_data<R3&>);
    static_assert(has_data<R3>); // borrowed range
    static_assert(has_data<const R3&>);
    static_assert(has_data<const R3>); // borrowed range

    R3 r;
    const R3& c = r;
    // PR libstdc++/100824
    // ranges::data should treat the subexpression as an lvalue
    assert(xranges::data(std::move(r)) == &R3::i);
    assert(xranges::data(std::move(c)) == &R3::l);

    // PR libstdc++/100824 comment 3
    // Check for member data() should use decay-copy
    RXX_DISABLE_WARNING_PUSH()
#if RXX_COMPILER_CLANG
    RXX_DISABLE_WARNING("-Wundefined-internal")
#endif
    struct A {
        int*&& data();
    };
    RXX_DISABLE_WARNING_POP()
    static_assert(has_data<A&>);
}

int main() {
    test01();
    test02();
    test03();
}
