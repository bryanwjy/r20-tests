// Copyright 2025-2026 Bryan Wong
// Adapted from GCC testsuite

#include "rxx/optional.h"

// PR libstdc++/109242
// transform omits required std::remove_cv_t from return optional type

struct A {};
struct B {};
struct C {};
struct D {};

struct F {
    A const operator()(int&);
    B const operator()(int const&);
    C const operator()(int&&);
    D const operator()(int const&&);
} f;

__RXX optional<int> o;
auto const& co = o;

auto o1 = o.transform(f);
static_assert(std::is_same_v<decltype(o1), __RXX optional<A>>);

auto o2 = co.transform(f);
static_assert(std::is_same_v<decltype(o2), __RXX optional<B>>);

auto o3 = std::move(o).transform(f);
static_assert(std::is_same_v<decltype(o3), __RXX optional<C>>);

auto o4 = std::move(co).transform(f);
static_assert(std::is_same_v<decltype(o4), __RXX optional<D>>);
