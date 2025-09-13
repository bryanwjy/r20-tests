// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/generator.h"

#if RXX_SUPPORTS_GENERATOR

template <typename... Ts>
using xgenerator = rxx::generator<Ts...>;

xgenerator<int> generator();

void try_reassigning() {
    auto gen = generator();
    auto gen2 = generator();
    gen = std::move(gen2);
}
#endif
