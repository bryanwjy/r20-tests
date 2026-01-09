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

// constexpr optional<T>& operator=(const optional<T>& rhs);

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

struct X {
    static bool throw_now;

    X() = default;
    X(X const&) {
        if (throw_now)
            RXX_THROW(6);
    }
    X& operator=(X const&) = default;
};

bool X::throw_now = false;

template <class Tp>
constexpr bool assign_empty(optional<Tp>&& lhs) {
    optional<Tp> const rhs;
    lhs = rhs;
    return !lhs.has_value() && !rhs.has_value();
}

template <class Tp>
constexpr bool assign_value(optional<Tp>&& lhs) {
    optional<Tp> const rhs(101);
    lhs = rhs;
    return lhs.has_value() && rhs.has_value() && *lhs == *rhs;
}

int main(int, char**) {
    {
        using O = optional<int>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = optional<TrivialTestTypes::TestType>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = optional<TestTypes::TestType>;
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> opt(3);
        optional<T> const opt2;
        assert(T::alive == 1);
        opt = opt2;
        assert(T::alive == 0);
        assert(!opt2.has_value());
        assert(!opt.has_value());
    }
#if RXX_WITH_EXCEPTIONS
    {
        optional<X> opt;
        optional<X> opt2(X{});
        assert(static_cast<bool>(opt2) == true);
        try {
            X::throw_now = true;
            opt = opt2;
            assert(false);
        } catch (int i) {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
    }
#endif

    return 0;
}
