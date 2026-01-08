// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2018-2026 Free Software Foundation, Inc.
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

// { dg-do compile { target c++17 } }
// { dg-options "-ftemplate-depth=270" }

#include "rxx/variant.h"

#include <limits>
#include <utility>

template <std::size_t I>
struct S {
    ~S() {}
};

template <std::size_t... Is>
void f_impl(std::index_sequence<Is...>) {
    using V = __RXX variant<S<Is>...>;

    // For a variant of 255 alternatives the valid indices are [0,254]
    // and index 255 means valueless-by-exception, so fits in one byte.
    if constexpr (__RXX variant_size_v<V> <=
        std::numeric_limits<unsigned char>::max())
        static_assert(sizeof(V) == 1);
    else
        static_assert(sizeof(V) > 1);
}

template <std::size_t I>
void f() {
    f_impl(std::make_index_sequence<I>{});
}

int main() {
    f<std::numeric_limits<unsigned char>::max() - 1>();
    f<std::numeric_limits<unsigned char>::max()>();
    f<std::numeric_limits<unsigned char>::max() + 1>();
}
