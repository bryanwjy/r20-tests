// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

void test01() {
    __RXX optional<int> o;
    (void)o.or_else([&] { return o; }); // OK
    (void)o.or_else(
        [] { return __RXX optional<short>(); }); // { dg-error "here" }
    (void)o.or_else([] { return 1; });            // { dg-error "here" }
    (void)std::move(o).or_else(
        [] { return __RXX optional<short>(); }); // { dg-error "here" }
    (void)std::move(o).or_else([] { return 1; }); // { dg-error "here" }
}

// { dg-prune-output "static assertion failed" }

void test02() {
    struct move_only {
        move_only() {}
        move_only(move_only&&) {}
    };

    __RXX optional<move_only> mo;
    (void)mo.or_else([] {
        return __RXX optional<move_only>{};
    }); // { dg-error "no matching function" }
        // { dg-error "use of deleted function" "" { target *-*-* } 0 }
}
