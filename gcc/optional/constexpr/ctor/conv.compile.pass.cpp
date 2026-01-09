// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>

constexpr bool test_cons() {
    __RXX optional<int> oi(1);
    __RXX optional<long> ol(oi);
    assert(*ol == 1L);
    assert(*oi == 1);

    __RXX optional<unsigned> ou(std::move(oi));
    assert(*ou == 1u);
    assert(oi.has_value() && *oi == 1);

    return true;
}

static_assert(test_cons());
