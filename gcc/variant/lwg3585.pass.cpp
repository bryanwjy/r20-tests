// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// LWG 3585. Variant converting assignment with immovable alternative

#include "rxx/variant.h"

#include <string>

struct A {
    A() = default;
    A(A&&) = delete;
};

int main() {
    __RXX variant<A, std::string> v;
    v = "hello";
}
