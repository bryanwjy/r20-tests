// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

struct NonEmpty {
    int x;
};
struct NonTrivial {
    constexpr NonTrivial() : x(0) {}
    NonTrivial(int p) : x(p) {}
    ~NonTrivial() {}

    int x;
};

struct TrivialEmpty {};
struct NonTrivialEmpty {
    NonTrivialEmpty() = default;
    NonTrivialEmpty(float) {}
    ~NonTrivialEmpty() {}
};

__RXX variant<NonEmpty> vNonEmpty(std::in_place_type<NonEmpty>);
// { dg-final { scan-tree-dump-not "__RXX variant<NonEmpty>::variant" "gimple" }
// }

__RXX variant<NonTrivial> vNonTrivial(std::in_place_type<NonTrivial>);
// { dg-final { scan-tree-dump-not "__RXX variant<NonTrivial>::variant" "gimple"
// } }

__RXX variant<int, NonTrivial> vNonTrivialNonConstexpr(
    std::in_place_index<1>, 2);
// { dg-final { scan-tree-dump "__RXX variant<int, NonTrivial>::variant"
// "gimple" } }

__RXX variant<TrivialEmpty> vTrivialEmpty(std::in_place_type<TrivialEmpty>);
// { dg-final { scan-tree-dump-not "__RXX variant<TrivialEmpty>::variant"
// "gimple" } }

__RXX variant<NonTrivialEmpty> vNonTrivialEmpty(
    std::in_place_type<NonTrivialEmpty>);
// { dg-final { scan-tree-dump-not "__RXX variant<NonTrivialEmpty>::variant"
// "gimple" } }

__RXX variant<int, NonTrivialEmpty> vNonTrivialEmptyNonConstexpr(
    std::in_place_index<1>, 2.0);
// { dg-final { scan-tree-dump "__RXX variant<int, NonTrivialEmpty>::variant"
// "gimple" } }
