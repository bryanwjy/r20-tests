// Copyright 2025-2026 Bryan Wong

#include "rxx/optional.h"

struct nothing {};

static_assert(sizeof(__RXX nua::optional<nothing>) == 1);
static_assert(sizeof(__RXX gcc::optional<nothing>) == 1 + sizeof(bool));
struct A {
    int x_;
    int y_;
};

struct B : public A {
    int z_;
    short z2_;
    virtual ~B() = default;
};
static_assert(sizeof(__RXX nua::optional<B>) == sizeof(B));
static_assert(sizeof(__RXX gcc::optional<B>) > sizeof(B));

struct PaddedBool {
    alignas(1024) bool b = false;
};

#if !RXX_COMPILER_MSVC
static_assert(sizeof(__RXX nua::optional<PaddedBool>) == sizeof(PaddedBool));
#endif

static_assert(sizeof(__RXX gcc::optional<PaddedBool>) ==
    sizeof(PaddedBool) + alignof(PaddedBool));
