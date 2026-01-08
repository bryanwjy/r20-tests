// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright 2025-2026 Bryan Wong
// Adapted from LLVM testsuite
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: c++03, c++11, c++14

// <optional>

// constexpr optional(const T& v);

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

int main(int, char**) {
    {
        typedef int T;
        constexpr T t(5);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
    {
        typedef double T;
        constexpr T t(3);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
    {
        int const x = 42;
        optional<int const> o(x);
        assert(*o == x);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        T const t(3);
        optional<T> opt = t;
        assert(T::alive == 2);
        assert(T::copy_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ExplicitTestTypes::TestType T;
        static_assert(!std::is_convertible<T const&, optional<T>>::value, "");
        T::reset();
        T const t(3);
        optional<T> opt(t);
        assert(T::alive == 2);
        assert(T::copy_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr T t(3);
        constexpr optional<T> opt = {t};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
    {
        typedef ExplicitConstexprTestTypes::TestType T;
        static_assert(!std::is_convertible<T const&, optional<T>>::value, "");
        constexpr T t(3);
        constexpr optional<T> opt(t);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
#if RXX_WITH_EXCEPTIONS
    {
        struct Z {
            Z(int) {}
            Z(Z const&) { throw 6; }
        };
        typedef Z T;
        try {
            T const t(3);
            optional<T> opt(t);
            assert(false);
        } catch (int i) {
            assert(i == 6);
        }
    }
#endif

    return 0;
}
