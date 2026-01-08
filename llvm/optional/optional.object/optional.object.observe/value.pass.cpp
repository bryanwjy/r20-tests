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

// constexpr T& optional<T>::value() &;

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

using __RXX bad_optional_access;
using __RXX optional;

struct X {
    X() = default;
    X(X const&) = delete;
    constexpr int test() const& { return 3; }
    int test() & { return 4; }
    constexpr int test() const&& { return 5; }
    int test() && { return 6; }
};

struct Y {
    constexpr int test() & { return 7; }
};

constexpr int test() {
    optional<Y> opt{Y{}};
    return opt.value().test();
}

int main(int, char**) {
    {
        optional<X> opt;
        ((void)opt);
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X&);
    }
    {
        optional<X> opt;
        opt.emplace();
        assert(opt.value().test() == 4);
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        X x;
        optional<X&> opt{x};
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X&);
    }
#endif
#if RXX_WITH_EXCEPTIONS
    {
        optional<X> opt;
        try {
            (void)opt.value();
            assert(false);
        } catch (bad_optional_access const&) {}
    }
#endif
    static_assert(test() == 7, "");

    return 0;
}
