// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_reset() {
    __RXX optional<int> oi(1);
    oi.reset();
    assert(!oi.has_value());
    oi.reset();
    assert(!oi.has_value());

    return true;
}

static_assert(test_reset());
