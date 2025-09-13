// Copyright 2025 Bryan Wong
// Adapted from GCC testsuite

// LWG 3900. allocator_arg_t overloads of generator::promise_type::operator new
// should not be constrained

#include "rxx/generator.h"

#include <memory_resource>

namespace xpmr {
using rxx::pmr::generator;
}

xpmr::generator<int> bar(
    std::allocator_arg_t, std::pmr::memory_resource& mr) // { dg-error "here" }
{
    co_yield 3;
}

// { dg-error "static assertion failed" "" { target *-*-* } 0 }
// { dg-error "no matching function .*memory_resource&" "" { target *-*-* } 0 }
