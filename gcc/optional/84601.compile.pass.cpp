// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

#include <cassert>
#include <utility>

using pair_t = std::pair<int, int>;
using opt_t = __RXX optional<pair_t>;

static_assert(std::is_copy_constructible_v<opt_t::value_type>);
static_assert(std::is_copy_assignable_v<opt_t::value_type>);

static_assert(std::is_copy_assignable_v<opt_t>); // assertion fails.

class A {
    void f(opt_t const& opt) { _opt = opt; }

    opt_t _opt;
};
