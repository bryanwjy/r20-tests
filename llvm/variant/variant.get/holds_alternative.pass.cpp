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

// template <class T, class... Types>
// constexpr bool holds_alternative(const variant<Types...>& v) noexcept;

#include "../../static_asserts.h"
#include "rxx/variant.h"

int main(int, char**) {
    {
        using V = __RXX variant<int>;
        constexpr V v;
        static_assert(__RXX holds_alternative<int>(v), "");
    }
    {
        using V = __RXX variant<int, long>;
        constexpr V v;
        static_assert(__RXX holds_alternative<int>(v), "");
        static_assert(!__RXX holds_alternative<long>(v), "");
    }
    { // noexcept test
        using V = __RXX variant<int>;
        V const v;
        ASSERT_NOEXCEPT(__RXX holds_alternative<int>(v));
    }

    return 0;
}
