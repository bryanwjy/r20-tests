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

// constexpr optional(T&& v);

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

class Z {
public:
    Z(int) {}
    Z(Z&&) { RXX_THROW(6); }
};

int main(int, char**) {
    {
        typedef int T;
        constexpr optional<T> opt(T(5));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        typedef double T;
        constexpr optional<T> opt(T(3));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        int const x = 42;
        optional<int const> o(std::move(x));
        assert(*o == 42);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        optional<T> opt = T{3};
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ExplicitTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, optional<T>>::value, "");
        T::reset();
        optional<T> opt(T{3});
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        optional<T> opt = {3};
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        assert(T::move_constructed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr optional<T> opt = {T(3)};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr optional<T> opt = {3};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T const&) {}
        };
    }
    {
        typedef ExplicitConstexprTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, optional<T>>::value, "");
        constexpr optional<T> opt(T{3});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor : public optional<T> {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
#if RXX_WITH_EXCEPTIONS
    {
        try {
            Z z(3);
            optional<Z> opt(std::move(z));
            assert(false);
        } catch (int i) {
            assert(i == 6);
        }
    }
#endif

    return 0;
}
