// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

struct A {
    A(int);
};
struct B {
    B() {};
};

void f(__RXX variant<A>);
int f(B);

int unambiguous = f({}); // { dg-bogus "ambiguous" }
