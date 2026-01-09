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

// constexpr T* optional<T>::operator->();

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

using __RXX optional;

struct X {
    int test() noexcept { return 3; }
    int test() const noexcept { return 3; }
};

struct Y {
    constexpr int test() { return 3; }
};

constexpr int test() {
    optional<Y> opt{Y{}};
    return opt->test();
}

int main(int, char**) {
    {
        __RXX optional<X> opt;
        ((void)opt);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X*);
        ASSERT_NOEXCEPT(opt.operator->());
    }
    {
        optional<X> opt(X{});
        assert(opt->test() == 3);
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        X x{};
        __RXX optional<X&> opt(x);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X*);
        ASSERT_NOEXCEPT(opt.operator->());
    }
    {
        X x{};
        __RXX optional<X const&> opt(x);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X const*);
        ASSERT_NOEXCEPT(opt.operator->());
    }
    {
        X x{};
        optional<X&> opt{x};
        assert(opt->test() == 3);
    }
    {
        X x{};
        optional<X const&> opt{x};
        assert(opt->test() == 3);
    }
#endif
    { static_assert(test() == 3, ""); }

    return 0;
}
