// Copyright 2025 Bryan Wong
// Adapted from LLVM testsuite

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <variant>

// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>&
//   get(variant<Types...>& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>>&&
//   get(variant<Types...>&& v);
// template <size_t I, class... Types>
//   constexpr variant_alternative_t<I, variant<Types...>> const& get(const
//   variant<Types...>& v);
// template <size_t I, class... Types>
//  constexpr variant_alternative_t<I, variant<Types...>> const&& get(const
//  variant<Types...>&& v);

#include "../../static_asserts.h"
#include "rxx/variant.h"

#include <cassert>
#include <type_traits>
#include <utility>

void test_const_lvalue_get() {
    {
        using V = __RXX variant<int, long const>;
        constexpr V v(42);
        ASSERT_NOT_NOEXCEPT(__RXX get<0>(v));
        ASSERT_SAME_TYPE(decltype(__RXX get<0>(v)), int const&);
        static_assert(__RXX get<0>(v) == 42, "");
    }
    {
        using V = __RXX variant<int, long const>;
        V const v(42);
        ASSERT_NOT_NOEXCEPT(__RXX get<0>(v));
        ASSERT_SAME_TYPE(decltype(__RXX get<0>(v)), int const&);
        assert(__RXX get<0>(v) == 42);
    }
    {
        using V = __RXX variant<int, long const>;
        constexpr V v(42l);
        ASSERT_NOT_NOEXCEPT(__RXX get<1>(v));
        ASSERT_SAME_TYPE(decltype(__RXX get<1>(v)), long const&);
        static_assert(__RXX get<1>(v) == 42, "");
    }
    {
        using V = __RXX variant<int, long const>;
        V const v(42l);
        ASSERT_NOT_NOEXCEPT(__RXX get<1>(v));
        ASSERT_SAME_TYPE(decltype(__RXX get<1>(v)), long const&);
        assert(__RXX get<1>(v) == 42);
    }
}

void test_lvalue_get() {
    {
        using V = __RXX variant<int, long const>;
        V v(42);
        ASSERT_NOT_NOEXCEPT(__RXX get<0>(v));
        ASSERT_SAME_TYPE(decltype(__RXX get<0>(v)), int&);
        assert(__RXX get<0>(v) == 42);
    }
    {
        using V = __RXX variant<int, long const>;
        V v(42l);
        ASSERT_SAME_TYPE(decltype(__RXX get<1>(v)), long const&);
        assert(__RXX get<1>(v) == 42);
    }
}

void test_rvalue_get() {
    {
        using V = __RXX variant<int, long const>;
        V v(42);
        ASSERT_NOT_NOEXCEPT(__RXX get<0>(std::move(v)));
        ASSERT_SAME_TYPE(decltype(__RXX get<0>(std::move(v))), int&&);
        assert(__RXX get<0>(std::move(v)) == 42);
    }
    {
        using V = __RXX variant<int, long const>;
        V v(42l);
        ASSERT_SAME_TYPE(decltype(__RXX get<1>(std::move(v))), long const&&);
        assert(__RXX get<1>(std::move(v)) == 42);
    }
}

void test_const_rvalue_get() {
    {
        using V = __RXX variant<int, long const>;
        V const v(42);
        ASSERT_NOT_NOEXCEPT(__RXX get<0>(std::move(v)));
        ASSERT_SAME_TYPE(decltype(__RXX get<0>(std::move(v))), int const&&);
        assert(__RXX get<0>(std::move(v)) == 42);
    }
    {
        using V = __RXX variant<int, long const>;
        V const v(42l);
        ASSERT_SAME_TYPE(decltype(__RXX get<1>(std::move(v))), long const&&);
        assert(__RXX get<1>(std::move(v)) == 42);
    }
}

template <std::size_t I>
using Idx = std::integral_constant<std::size_t, I>;

void test_throws_for_all_value_categories() {
#if RXX_WITH_EXCEPTIONS
    using V = __RXX variant<int, long>;
    V v0(42);
    V const& cv0 = v0;
    assert(v0.index() == 0);
    V v1(42l);
    V const& cv1 = v1;
    assert(v1.index() == 1);
    std::integral_constant<std::size_t, 0> zero;
    std::integral_constant<std::size_t, 1> one;
    auto test = [](auto idx, auto&& v) {
        using Idx = decltype(idx);
        try {
            (void) __RXX get<Idx::value>(std::forward<decltype(v)>(v));
        } catch (__RXX bad_variant_access const&) {
            return true;
        } catch (...) { /* ... */
        }
        return false;
    };
    { // lvalue test cases
        assert(test(one, v0));
        assert(test(zero, v1));
    }
    { // const lvalue test cases
        assert(test(one, cv0));
        assert(test(zero, cv1));
    }
    { // rvalue test cases
        assert(test(one, std::move(v0)));
        assert(test(zero, std::move(v1)));
    }
    { // const rvalue test cases
        assert(test(one, std::move(cv0)));
        assert(test(zero, std::move(cv1)));
    }
#endif
}

int main(int, char**) {
    test_const_lvalue_get();
    test_lvalue_get();
    test_rvalue_get();
    test_const_rvalue_get();
    test_throws_for_all_value_categories();

    return 0;
}
