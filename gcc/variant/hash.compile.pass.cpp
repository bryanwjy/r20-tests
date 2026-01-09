// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Copyright (C) 2016-2026 Free Software Foundation, Inc.
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

#include "rxx/variant.h"

#include <cassert>

class S {}; // No hash specialization

template <class T>
auto f(int) -> decltype(std::hash<__RXX variant<T>>(), std::true_type());

template <class T>
auto f(...) -> decltype(std::false_type());

static_assert(!decltype(f<S>(0))::value, "");
static_assert(!decltype(f<__RXX variant<S>>(0))::value, "");
static_assert(!decltype(f<__RXX variant<S, S>>(0))::value, "");
static_assert(decltype(f<__RXX variant<int>>(0))::value, "");
static_assert(decltype(f<__RXX variant<int, int>>(0))::value, "");
static_assert(!std::is_invocable_v<std::hash<__RXX variant<S>>&,
    __RXX variant<S> const&>);
static_assert(!std::is_invocable_v<std::hash<__RXX variant<S, int>>&,
    __RXX variant<S, int> const&>);
static_assert(std::is_invocable_v<std::hash<__RXX variant<int>>&,
    __RXX variant<int> const&>);
static_assert(std::is_invocable_v<std::hash<__RXX variant<int, int>>&,
    __RXX variant<int, int> const&>);

int main() {
    int x = 42;
    __RXX variant<int> x2 = 42;
    assert(std::hash<int>()(x) == std::hash<__RXX variant<int>>()(x2));
}

// Check for presence/absence of nested types.

template <typename T>
using res_type = typename std::hash<T>::result_type;
template <typename T>
using arg_type = typename std::hash<T>::argument_type;

template <typename Variant, typename = void>
constexpr bool has_res_type = false;
template <typename Variant>
constexpr bool has_res_type<Variant, std::void_t<res_type<Variant>>> = true;
template <typename Variant, typename = void>
constexpr bool has_arg_type = false;
template <typename Variant>
constexpr bool has_arg_type<Variant, std::void_t<arg_type<Variant>>> = true;

template <typename... Ts>
constexpr bool has_no_types = !has_res_type<__RXX variant<Ts...>> &&
    !has_arg_type<__RXX variant<Ts...>>;

#if __cplusplus >= 202002L
// Nested types result_type and argument_type are not present in C++20
static_assert(has_no_types<int>);
static_assert(has_no_types<int, double>);
#else
// Nested types result_type and argument_type are deprecated in C++17.
using R1 =
    std::hash<__RXX variant<int>>::result_type; // { dg-warning "deprecated"
                                                  // "" { target c++17_only } }
using A1 =
    std::hash<__RXX variant<int>>::argument_type; // { dg-warning "deprecated"
                                                    // "" { target c++17_only }
                                                    // }
using R2 = std::hash<__RXX variant<char,
    int>>::result_type; // { dg-warning "deprecated" "" { target c++17_only } }
using A2 = std::hash<__RXX variant<char, int>>::
    argument_type; // { dg-warning "deprecated" "" { target c++17_only } }
#endif

// Disabled specializations do not have the nested types.
static_assert(has_no_types<S>);
static_assert(has_no_types<int, S>);
