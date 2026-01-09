// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/variant.h"

// P2062R2 Inheriting from __RXX variant (resolving LWG 3052)

struct V : __RXX variant<int> {
    using __RXX variant<int>::variant;
};

constexpr int test01() {
    V v = 42;
    return __RXX visit([](int&) { return 17; }, v);
}
static_assert(test01() == 17);

constexpr int test02() {
    V const c = 77;
    __RXX variant<char*, long> x = 88L;
    return __RXX visit(
        [](auto&& a, auto&& b) {
            if constexpr (std::is_same_v<decltype(a), int const&&>)
                if constexpr (std::is_same_v<decltype(b), long&&>)
                    return 99;
            return 0;
        },
        std::move(c), std::move(x));
}
static_assert(test02() == 99);
