// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include <variant>

__RXX variant<> v0;            // { dg-error "here" }
// { dg-error "must have at least one alternative" "" { target *-*-* } 0 }
__RXX variant<int, void> v1; // { dg-error "here" }
__RXX variant<int, void const> v2; // { dg-error "here" }
__RXX variant<int, int&> v3; // { dg-error "here" }
__RXX variant<int, void()> v4; // { dg-error "here" }
__RXX variant<int, int[]> v5; // { dg-error "here" }
__RXX variant<int, int[1]> v6; // { dg-error "here" }
// { dg-error "must be non-array object types" "" { target *-*-* } 0 }

// All of variant's base classes are instantiated before checking any
// static_assert, so we get lots of errors before the expected errors above.
// { dg-excess-errors "" }
