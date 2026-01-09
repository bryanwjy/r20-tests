// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// PR 117858 __RXX optional with a constructor template<typename T> ctor(T)
// PR 117889 Failure to build qtwebengine-6.8.1

#include "rxx/optional.h"

struct Focus {
    template <class T>
    Focus(T) {}
};

void test_pr117858(__RXX optional<Focus>& f) {
    f = f;
    f = std::move(f);
}

void test_pr117889(__RXX optional<Focus>& f) {
    __RXX optional<Focus> f2 = f;
    __RXX optional<Focus> f3 = std::move(f);
}
