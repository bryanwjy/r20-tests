// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

#include <cassert>

struct NonEmpty {
    int x;
};
struct NonTrivial {
    constexpr NonTrivial() : x(0) {}
    constexpr NonTrivial(int p) : x(p) {}
    ~NonTrivial() {}

    int x;
};

struct TrivialEmpty {};
struct NonTrivialEmpty {
    ~NonTrivialEmpty() {}
};

template <typename T>
struct Compose : T {
    __RXX variant<T, int> v;
};

template <typename T>
bool testAlias() {
    Compose<T> c;
    return static_cast<T*>(&c) == &__RXX get<T>(c.v);
}

int main() {
    assert(!testAlias<NonEmpty>());
    assert(!testAlias<NonTrivial>());
    assert(!testAlias<TrivialEmpty>());
    assert(!testAlias<NonTrivialEmpty>());
}
