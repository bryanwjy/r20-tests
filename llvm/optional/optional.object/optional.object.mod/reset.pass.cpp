// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <optional>

// void reset() noexcept;

#include "rxx/optional.h"

#include <cassert>

using __RXX optional;

struct X {
    static bool dtor_called;
    X() = default;
    X(X const&) = default;
    X& operator=(X const&) = default;
    ~X() { dtor_called = true; }
};

bool X::dtor_called = false;

constexpr bool check_reset() {
    {
        optional<int> opt;
        static_assert(noexcept(opt.reset()) == true, "");
        opt.reset();
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<int> opt(3);
        opt.reset();
        assert(static_cast<bool>(opt) == false);
    }
    return true;
}

int main(int, char**) {
    check_reset();
    static_assert(check_reset());
    {
        optional<X> opt;
        static_assert(noexcept(opt.reset()) == true, "");
        assert(X::dtor_called == false);
        opt.reset();
        assert(X::dtor_called == false);
        assert(static_cast<bool>(opt) == false);
    }
    {
        optional<X> opt(X{});
        X::dtor_called = false;
        opt.reset();
        assert(X::dtor_called == true);
        assert(static_cast<bool>(opt) == false);
        X::dtor_called = false;
    }

#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        X x{};
        optional<X&> opt(x);
        X::dtor_called = false;
        opt.reset();
        assert(X::dtor_called == false);
        assert(static_cast<bool>(opt) == false);
    }
#endif

    return 0;
}
