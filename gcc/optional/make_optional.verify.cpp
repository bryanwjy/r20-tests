// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <initializer_list>

struct S {
    int x;
    int* p;
};
int v;

auto os1 = __RXX make_optional<S>(
    {1, &v}); // { dg-error "no matching function for" "" { target c++26 } }

struct Cont {
    Cont();
    Cont(std::initializer_list<int>, int);
};

auto oc1 = __RXX make_optional<Cont>(
    {}); // { dg-error "no matching function for" "" { target c++26 } }

// { dg-prune-output "no type named 'type' in 'struct std::enable_if" }
// { dg-prune-output "type/value mismatch at argument 1 in template parameter
// list" }
