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

auto lr1 = __RXX make_optional<C&>(s); // changed meaning
static_assert(std::is_same_v<decltype(lr1), __RXX optional<decay_pre26<C&>>>);
auto lr2 = __RXX make_optional<C const&>(
    s); // { dg-error "here" "" { target c++23_down } }
auto lr3 = __RXX make_optional<C&&>(
    s); // { dg-error "no matching function for call" }
auto lr4 = __RXX make_optional<C const&&>(
    s); // { dg-error "no matching function for call" }

auto clr1 = __RXX make_optional<C&>(
    cs); // { dg-error "no matching function for call" }
auto clr2 = __RXX make_optional<C const&>(cs); // changed meaning
static_assert(
    std::is_same_v<decltype(clr2), __RXX optional<decay_pre26<C const&>>>);
auto clr3 = __RXX make_optional<C&&>(
    cs); // { dg-error "no matching function for call" }
auto clr3 = __RXX make_optional<C const&&>(
    cs); // { dg-error "no matching function for call" }

// { dg-prune-output "no type named 'type' in 'struct std::enable_if" }
// { dg-prune-output "type/value mismatch at argument 1 in template parameter
// list" } { dg-prune-output "in a union may not have reference type" } {
// dg-prune-output "static assertion failed" } { dg-prune-output "forming
// pointer to reference type" } { dg-prune-output "cannot bind .* reference of
// type" } { dg-prune-output "binding reference of type" } { dg-prune-output "no
// matching function for call to `__RXX optional" }
