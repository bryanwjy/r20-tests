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
//
// template <class T>
//   constexpr optional<decay_t<T>> make_optional(T&& v);

#include "../../static_asserts.h"
#include "rxx/optional.h"

#include <cassert>
#include <memory>
#include <string>

int main(int, char**) {
    {
        int arr[10];
        auto opt = __RXX make_optional(arr);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int*>);
        assert(*opt == arr);
    }
    {
        constexpr auto opt = __RXX make_optional(2);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int> const);
        static_assert(opt.value() == 2);
    }
    {
        auto opt = __RXX make_optional(2);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<int>);
        assert(*opt == 2);
    }
    {
        std::string const s = "123";
        auto opt = __RXX make_optional(s);
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<std::string>);
        assert(*opt == "123");
    }
    {
        std::unique_ptr<int> s = std::make_unique<int>(3);
        auto opt = __RXX make_optional(std::move(s));
        ASSERT_SAME_TYPE(decltype(opt), __RXX optional<std::unique_ptr<int>>);
        assert(**opt == 3);
        assert(s == nullptr);
    }

    return 0;
}
