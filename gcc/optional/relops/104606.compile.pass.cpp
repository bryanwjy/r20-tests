// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// Bug 104606 comparison operator resolution with __RXX optional and -std=c++20

#include "rxx/optional.h"

#include <variant>
#include <vector>

struct Value : std::variant<std::vector<Value>> {};

struct Comparator {
    template <typename T>
    bool operator<=(T const&) {
        return true;
    }
};

__RXX optional<Value> o;
Comparator c;

auto x = c <= o;
