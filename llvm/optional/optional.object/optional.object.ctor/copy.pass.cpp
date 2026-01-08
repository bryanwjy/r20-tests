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

// constexpr optional(const optional<T>& rhs);

#include "../../../archetypes.h"
#include "rxx/optional.h"

#include <cassert>
#include <type_traits>

using __RXX optional;

template <class T, class... InitArgs>
void test(InitArgs&&... args) {
    optional<T> const rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
}

template <class T, class... InitArgs>
constexpr bool constexpr_test(InitArgs&&... args) {
    static_assert(std::is_trivially_copy_constructible_v<T>, ""); // requirement
    optional<T> const rhs(std::forward<InitArgs>(args)...);
    optional<T> lhs = rhs;
    return (lhs.has_value() == rhs.has_value()) &&
        (lhs.has_value() ? *lhs == *rhs : true);
}

void test_throwing_ctor() {
#if RXX_WITH_EXCEPTIONS
    struct Z {
        Z() : count(0) {}
        Z(Z const& o) : count(o.count + 1) {
            if (count == 2)
                throw 6;
        }
        int count;
    };
    Z const z;
    optional<Z> const rhs(z);
    try {
        optional<Z> lhs(rhs);
        assert(false);
    } catch (int i) {
        assert(i == 6);
    }
#endif
}

template <class T, class... InitArgs>
void test_ref(InitArgs&&... args) {
    optional<T> const rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(&(*lhs) == &(*rhs));
}

void test_reference_extension() {
#if defined(_LIBCPP_VERSION) && \
    0 // FIXME these extensions are currently disabled.
    using T = TestTypes::TestType;
    T::reset();
    {
        T t;
        T::reset_constructors();
        test_ref<T&>();
        test_ref<T&>(t);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::destroyed == 1);
    assert(T::alive == 0);
    {
        T t;
        T const& ct = t;
        T::reset_constructors();
        test_ref<T const&>();
        test_ref<T const&>(t);
        test_ref<T const&>(ct);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        static_assert(
            !std::is_copy_constructible<__RXX optional<T&&>>::value, "");
        static_assert(
            !std::is_copy_constructible<__RXX optional<T const&&>>::value,
            "");
    }
#endif
}

int main(int, char**) {
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>(), "");
    static_assert(constexpr_test<int>(3), "");

    {
        optional<int const> const o(42);
        optional<int const> o2(o);
        assert(*o2 == 42);
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> const rhs;
        assert(T::alive == 0);
        optional<T> const lhs(rhs);
        assert(lhs.has_value() == false);
        assert(T::alive == 0);
    }
    TestTypes::TestType::reset();
    {
        using T = TestTypes::TestType;
        T::reset();
        optional<T> const rhs(42);
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        optional<T> const lhs(rhs);
        assert(lhs.has_value());
        assert(T::copy_constructed == 1);
        assert(T::alive == 2);
    }
    TestTypes::TestType::reset();
    {
        using namespace ConstexprTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        using namespace TrivialTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    { test_throwing_ctor(); }
    { test_reference_extension(); }
    {
        constexpr __RXX optional<int> o1{4};
        constexpr __RXX optional<int> o2 = o1;
        static_assert(*o2 == 4, "");
    }

    // LWG3836 https://wg21.link/LWG3836
    // __RXX optional<bool> conversion constructor optional(const optional<U>&)
    // should take precedence over optional(U&&) with operator bool
    {
        __RXX optional<bool> o1(false);
        __RXX optional<bool> o2(o1);
        assert(!o2.value());
    }

    return 0;
}
