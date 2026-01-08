// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// LWG 3746. optional's spaceship with U with a type derived from optional
// causes infinite constraint meta-recursion

#include "rxx/optional.h"

struct S : __RXX optional<char> {
    bool operator==(S const&) const;
    bool operator<(S const&) const;
    bool operator>(S const&) const;
    bool operator<=(S const&) const;
    bool operator>=(S const&) const;
};

auto cmp(S const& s, __RXX optional<char> const& o) {
    return s <=> o;
}
