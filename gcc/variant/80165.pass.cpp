// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

#include <tuple>

int main() {
    using variant_t = __RXX variant<short, int, long>;
    constexpr auto variant_v = variant_t{std::in_place_index_t<0>{}, short{}};
    constexpr auto tuple = std::make_tuple(variant_v);
    constexpr std::tuple tuple_v{variant_v};
}
