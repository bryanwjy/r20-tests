// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

void test01() {
    struct X {
        ~X() {} // non-trivial
    };

    __RXX variant<int const, X const> v;
    auto vv = v;
}

constexpr bool test02() {
    struct Y {
        constexpr ~Y() {} // non-trivial
    };
    using V = __RXX variant<int, int const, Y const, Y>;

    V v1(std::in_place_index<1>, 1);
    V vv1 = v1;
    if (vv1.index() != v1.index())
        return false;

    V v2(std::in_place_index<2>);
    V vv2 = v2;
    if (vv2.index() != v2.index())
        return false;

    return true;
}
static_assert(test02());

constexpr bool test03() {
    struct Y {
        constexpr ~Y() {} // non-trivial
    };
    using V = __RXX variant<int, int, Y, Y>;

    V v1(std::in_place_index<1>, 1);
    V vv1 = v1;
    if (vv1.index() != v1.index())
        return false;
    vv1 = v1;
    if (vv1.index() != v1.index())
        return false;
    vv1 = std::move(v1);
    if (vv1.index() != v1.index())
        return false;

    V v2(std::in_place_index<2>);
    V vv2 = v2;
    if (vv2.index() != v2.index())
        return false;
    vv2 = v2;
    if (vv2.index() != v2.index())
        return false;
    vv2 = std::move(v2);
    if (vv2.index() != v2.index())
        return false;

    return true;
}
static_assert(test03());
