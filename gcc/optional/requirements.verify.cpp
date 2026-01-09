// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

// C++ < 26:
// T shall be a type other than cv in_place_t or cv nullopt_t
// that meets the Cpp17Destructible requirements
// C++ >= 26:
// A type X is a valid contained type for optional if X is an lvalue reference
// type or a complete non-array object type, and remove_cvref_t<X> is a type
// other than in_place_t or nullopt_t. If a specialization of optional
// is instantiated with a type T that is not a valid contained type for
// optional, the program is ill-formed. If T is an object type,
// T shall meet the Cpp17Destructible requirements

__RXX optional<__RXX nullopt_t> o1; // { dg-error "here" }
__RXX optional<__RXX nullopt_t const> o2; // { dg-error "here" }
__RXX optional<std::in_place_t> o3; // { dg-error "here" }
__RXX optional<std::in_place_t const> o4; // { dg-error "here" }
__RXX optional<int&> o5;         // { dg-error "here" "optional<T&> is a C++26
                           // feature" { target c++23_down } }
__RXX optional<int[1]> o6; // { dg-error "here" }
__RXX optional<int[]> o7; // { dg-error "here" }
__RXX optional<int()> o8; // { dg-error "here" }
__RXX optional<int const&> o9;   // { dg-error "here" "optional<T&> is a C++26
                                 // feature" { target c++23_down } }
__RXX optional<std::in_place_t&> o10; // { dg-error "here" }
__RXX optional<std::in_place_t const&> o11; // { dg-error "here" }
__RXX optional<__RXX nullopt_t&> o12; // { dg-error "here" }
__RXX optional<__RXX nullopt_t const&> o13; // { dg-error "here" }
__RXX optional<int&&> o14; // { dg-error "here" }
__RXX optional<int const&&> o15; // { dg-error "here" }

// { dg-error "static assertion failed" "" { target *-*-* } 0 }

// { dg-prune-output "forming pointer to reference type" }
// { dg-prune-output "union may not have reference type" }
// { dg-prune-output "function returning an array" }
// { dg-prune-output "flexible array member .* in union" }
// { dg-prune-output "function returning a function" }
// { dg-prune-output "invalidly declared function type" }
