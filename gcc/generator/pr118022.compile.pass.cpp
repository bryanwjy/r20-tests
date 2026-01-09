// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/generator.h"

#if RXX_SUPPORTS_GENERATOR

template <typename... Ts>
using xgenerator = __RXX generator<Ts...>;

struct O {
    O() = default;
    explicit O(O const&) = default;
};

xgenerator<O&&> gen() {
    O const o;
    co_yield o;
}

#endif
