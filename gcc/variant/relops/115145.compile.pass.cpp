// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// PR libstdc++/115145
// lambda in rewritten __RXX variant comparisons does not specify return type

#include "rxx/variant.h"

struct Bool {
    operator bool() & { return val; }
    bool const val;
};

Bool t{true}, f{false};

struct A {
    Bool& operator==(A const&) const { return t; }
    Bool& operator!=(A const&) const { return f; }
    Bool& operator<(A const&) const { return f; }
    Bool& operator>(A const&) const { return f; }
    Bool& operator<=(A const&) const { return t; }
    Bool& operator>=(A const&) const { return t; }
};

bool check_bool(bool);
template <typename T>
void check_bool(T) = delete;

void test_pr115145() {
    __RXX variant<A> v;
    check_bool(v == v);
    check_bool(!(v != v));
    check_bool(!(v < v));
    check_bool(!(v > v));
    check_bool(v <= v);
    check_bool(v >= v);
}
