// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_swap() {
    __RXX optional<int> o0, o1(1);
    o0.swap(o1);
    assert(*o0 == 1);
    assert(!o1.has_value());
    o0.swap(o1);
    assert(!o0.has_value());
    assert(*o1 == 1);
    o0.swap(o0);
    assert(!o0.has_value());
    o1.swap(o1);
    assert(*o1 == 1);
    __RXX optional<int> o2(2);
    swap(o1, o2);
    assert(*o1 == 2);
    assert(*o2 == 1);

    return true;
}

static_assert(test_swap());
