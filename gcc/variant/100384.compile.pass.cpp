// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h" // IWYU pragma: keep

int visit(int*, std::true_type) {
    return 0;
}

std::true_type const dat;

int i = visit(nullptr, dat);
