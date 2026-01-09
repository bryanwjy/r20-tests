// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

// LWG 2762 adds noexcept to operator-> and operator*
#include "rxx/optional.h"

struct S {
    void can_throw();
    void cannot_throw() noexcept;
};

static_assert(!noexcept(std::declval<__RXX optional<S>&>()->can_throw()));
static_assert(noexcept(std::declval<__RXX optional<S>&>()->cannot_throw()));

static_assert(noexcept(std::declval<__RXX optional<S>&>().operator->()));
static_assert(noexcept(std::declval<__RXX optional<int>&>().operator->()));

static_assert(noexcept(*std::declval<__RXX optional<int>&>()));
static_assert(noexcept(*std::declval<__RXX optional<int> const&>()));
static_assert(noexcept(*std::declval<__RXX optional<int>&&>()));
static_assert(noexcept(*std::declval<__RXX optional<int> const&&>()));
