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

// constexpr const T& optional<T>::value() const &;

#include "../../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>

using __RXX bad_optional_access;
using __RXX optional;
using std::in_place;

struct X {
    X() = default;
    X(X const&) = delete;
    constexpr int test() const& { return 3; }
    int test() & { return 4; }
    constexpr int test() const&& { return 5; }
    int test() && { return 6; }
};

int main(int, char**) {
    {
        optional<X> const opt;
        ((void)opt);
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X const&);
    }
    {
        constexpr optional<X> opt(in_place);
        static_assert(opt.value().test() == 3, "");
    }
    {
        optional<X> const opt(in_place);
        assert(opt.value().test() == 3);
    }
#if RXX_WITH_EXCEPTIONS
    {
        optional<X> const opt;
        try {
            (void)opt.value();
            assert(false);
        } catch (bad_optional_access const&) {}
    }
#endif

    return 0;
}
