// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <type_traits>

struct C {
    C();
    C(int);
};
C s(10);
C const cs(1);

template <typename T>
using decay_pre26 =
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    T;
#else
    std::decay_t<T>;
#endif

auto p1 = __RXX make_optional<C&>(
    C(10)); // { dg-error "no matching function for call" }
auto p2 = __RXX make_optional<C const&>(C(10)); // { dg-error "from here" }
auto p3 = __RXX make_optional<C&&>(
    C(10)); // { dg-error "from here" "" { target c++26 } }
auto p4 = __RXX make_optional<C const&&>(C(10)); // { dg-error "from here" }

auto b1 = __RXX make_optional<C&>(
    {10}); // { dg-error "no matching function for call" }
auto b2 = __RXX make_optional<C const&>(
    {10}); // { dg-error "no matching function for call" "" { target c++26 } }
auto b3 = __RXX make_optional<C&&>(
    {10}); // { dg-error "no matching function for call" "" { target c++26 } }
auto b4 = __RXX make_optional<C const&&>(
    {10}); // { dg-error "no matching function for call" "" { target c++26 } }

// { dg-prune-output "no type named 'type' in 'struct std::enable_if" }
// { dg-prune-output "type/value mismatch at argument 1 in template parameter
// list" } { dg-prune-output "in a union may not have reference type" } {
// dg-prune-output "static assertion failed" } { dg-prune-output "forming
// pointer to reference type" } { dg-prune-output "cannot bind .* reference of
// type" } { dg-prune-output "binding reference of type" } { dg-prune-output "no
// matching function for call to '__RXX optional" }
