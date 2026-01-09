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

// constexpr const T& optional<T>::operator*() const &;

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

using __RXX optional;

struct X {
    constexpr int test() const& { return 3; }
    int test() & { return 4; }
    constexpr int test() const&& { return 5; }
    int test() && { return 6; }
};

struct Y {
    int test() const { return 2; }
};

int main(int, char**) {
    {
        optional<X> const opt;
        ((void)opt);
        ASSERT_SAME_TYPE(decltype(*opt), X const&);
        ASSERT_NOEXCEPT(*opt);
    }
    {
        constexpr optional<X> opt(X{});
        static_assert((*opt).test() == 3, "");
    }
#if RXX_SUPPORTS_OPTIONAL_REFERENCES
    {
        X x{};
        optional<X&> const opt{x};
        ASSERT_SAME_TYPE(decltype(*opt), X&);
        ASSERT_NOEXCEPT(*opt);
    }
    {
        X x{};
        optional<X const&> const opt{x};
        ASSERT_SAME_TYPE(decltype(*opt), X const&);
        ASSERT_NOEXCEPT(*opt);
    }
    {
        static constexpr X x{};
        constexpr optional<X const&> opt(x);
        static_assert((*opt).test() == 3);
    }
#endif
    {
        constexpr optional<Y> opt(Y{});
        assert((*opt).test() == 2);
    }

    return 0;
}
