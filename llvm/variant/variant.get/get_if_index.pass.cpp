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
//  constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>>
//   get_if(variant<Types...>* v) noexcept;
// template <size_t I, class... Types>
//  constexpr add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
//   get_if(const variant<Types...>* v) noexcept;

#include "../../static_asserts.h"
#include "rxx/variant.h"

#include <cassert>

void test_const_get_if() {
    {
        using V = __RXX variant<int>;
        constexpr V const* v = nullptr;
        static_assert(__RXX get_if<0>(v) == nullptr, "");
    }
    {
        using V = __RXX variant<int, long const>;
        constexpr V v(42);
        ASSERT_NOEXCEPT(__RXX get_if<0>(&v));
        ASSERT_SAME_TYPE(decltype(__RXX get_if<0>(&v)), int const*);
        static_assert(*__RXX get_if<0>(&v) == 42, "");
        static_assert(__RXX get_if<1>(&v) == nullptr, "");
    }
    {
        using V = __RXX variant<int, long const>;
        constexpr V v(42l);
        ASSERT_SAME_TYPE(decltype(__RXX get_if<1>(&v)), long const*);
        static_assert(*__RXX get_if<1>(&v) == 42, "");
        static_assert(__RXX get_if<0>(&v) == nullptr, "");
    }
}

void test_get_if() {
    {
        using V = __RXX variant<int>;
        V* v = nullptr;
        assert(__RXX get_if<0>(v) == nullptr);
    }
    {
        using V = __RXX variant<int, long>;
        V v(42);
        ASSERT_NOEXCEPT(__RXX get_if<0>(&v));
        ASSERT_SAME_TYPE(decltype(__RXX get_if<0>(&v)), int*);
        assert(*__RXX get_if<0>(&v) == 42);
        assert(__RXX get_if<1>(&v) == nullptr);
    }
    {
        using V = __RXX variant<int, long const>;
        V v(42l);
        ASSERT_SAME_TYPE(decltype(__RXX get_if<1>(&v)), long const*);
        assert(*__RXX get_if<1>(&v) == 42);
        assert(__RXX get_if<0>(&v) == nullptr);
    }
}

int main(int, char**) {
    test_const_get_if();
    test_get_if();

    return 0;
}
